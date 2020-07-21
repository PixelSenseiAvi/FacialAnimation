#include <stdio.h>
#include <iostream>
#include <unistd.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "blendshape_loader.h"
#include "blendshape_model.h"
#include "blendshape_modifier.h"
#include "blendshape_animator.h"
#include "ui_manipulator.h"
#include "glprogram.h"
#include "primitives.h"

#include "bvh.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "ui/imgui.h"
#include "ui/imgui_impl_glfw.h"
#include "ui/imgui_impl_opengl3.h"

#include "stopwatch.h"

int last_key,last_action;
int frame_width,frame_height;
double last_frame_time;

BlendshapeLoader* loader;
BlendshapeModel* model;
BlendshapeModifier* modifier;
BlendshapeAnimator* animator;
UIManipulator* uimanipulator;

render_engine::GLProgram shader;
render_engine::GLProgram debug_shader;

/// Camera parameters
glm::vec3 camera_pos(0.0f);
glm::mat4 projection_matrix(1.0f);
glm::mat4 view_matrix(1.0f);

/// Mouse drag
bool mouse_clicked = false;
bool update_from_modifier = false;
int mouse_button, mouse_action;
double last_xpos,last_ypos;
int triangle_index = -1;
int vertex_index = -1;
glm::vec3 clicked_vertex(0.0f);

void window_size_callback(GLFWwindow* window, int width, int height)
{
    printf("size changed \n");
    glfwGetFramebufferSize(window,&frame_width,&frame_height);
    glViewport(0,0,frame_width,frame_height);
}

float fov = 60.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    last_key = key;
    last_action = action;
    if(key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        fov++;
    }
    else if(key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        fov--;
    }

    printf("%f \n",fov);
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    last_xpos = xpos;
    last_ypos = ypos;
}

void drop_callback(GLFWwindow* window,int count, const char** paths)
{
    int i;
    for(i=0;i<count;i++)
    {
        printf("file path %s \n",paths[i]);
    }
}

void mouse_button_callback(GLFWwindow* window,int button, int action, int mods)
{
    mouse_button = button;
    mouse_action = action;
}

void setup_imgui(GLFWwindow* window)
{
    const char* gl_version = "#version 410";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init(gl_version);
}

void draw_ui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool show_sliders = false;
    ImGui::Checkbox("Enable sliders",&show_sliders);
    ImGui::Checkbox("Update from modifier",&update_from_modifier);
    /// Render all the UI
    if(ImGui::Button("Play Animation"))
    {
        animator->start_animation(glfwGetTime(),0.1f);
    }

    if(ImGui::Button("Stop Animation"))
    {
        animator->stop_animation();
    }

    if(!animator->is_playing && show_sliders)
    {
        uimanipulator->render_ui();
    }

    modifier->render_ui(uimanipulator->weights);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void shutdown_ui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void update_vertex(BlendShapeMesh& mesh,
                   const glm::vec3& newpos,
                   int vertex_index)
{
    mesh.transit_buffer[vertex_index] = newpos.x;
    mesh.transit_buffer[vertex_index + 1]= newpos.y;
    mesh.transit_buffer[vertex_index + 2] = newpos.z;
}

void manipulation()
{
    Ray ray;
    ray.update(projection_matrix,view_matrix,camera_pos,last_xpos,last_ypos,frame_width,frame_height);

    if(mouse_button == GLFW_MOUSE_BUTTON_1)
    {
        if(mouse_action == GLFW_PRESS)
        {
            std::cout << "Mouse pressed " << std::endl;
            
            /// Raycasting
            triangle_index = IntersectionFinder::get_intersection(ray,projection_matrix * view_matrix,
                                                      model->mesh,clicked_vertex,vertex_index);
            if(triangle_index != -1)
            {
                mouse_clicked = true;
                modifier->add_vertex(*loader,model->mesh.pindices[vertex_index/3],uimanipulator->weights);
            }

            /// Button pressed
            /// Get the vertex (only once)
            /// start modification loop
        }
        else
        {
            std::cout << "Mouse released " << std::endl;
            /// check if button was pressed on a vertex
            /// Button released
            /// stop modification loop
            /// Update the BVH

            modifier->end_drag();
            /// Reset
            mouse_clicked = false;
        }
    }

    mouse_button = -1;
    mouse_action = -1;

    if(mouse_clicked)
    {
        glm::vec3 cube_vec = clicked_vertex - ray.position;
        float dot = glm::dot(cube_vec,ray.direction);
        glm::vec3 new_pos = ray.position + dot * ray.direction;
        Primitives::draw_cube(projection_matrix*view_matrix,new_pos,debug_shader);

        modifier->calculate_weights(&new_pos[0],uimanipulator->weights,false);
        //update_vertex(model->mesh,new_pos,vertex_index);
    }
}

void draw()
{
    Stopwatch watch;
    if(animator->is_playing)
        animator->update(glfwGetTime());
    
    /// Update the blendshape at the beginning
    camera_pos = glm::vec3(0,15.0f,40.0f);
    glm::vec3 camera_dir(0,0,-1.0f);

    projection_matrix = glm::mat4(1.0f);
    view_matrix = glm::mat4(1.0f);
    projection_matrix = glm::perspective(glm::radians(fov),(float)frame_width/(float)frame_height,0.1f,100.0f);
    view_matrix = glm::lookAt(camera_pos,camera_pos + camera_dir,glm::vec3(0,1,0));

    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f));
    //model_matrix = glm::scale(model_matrix,glm::vec3(0.3f,0.3f,0.3f));
    glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

    render_engine::glprogram::use(shader.program);

    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_MVP], 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_M], 1, GL_FALSE, glm::value_ptr(model_matrix));

    /// Send light information
    auto it = shader.light_uniforms.find("l_point_light_pos");
    if(it != shader.light_uniforms.end())
        glUniform3fv(it->second.index,1,glm::value_ptr(camera_pos));

    glm::vec3 light_color(181.0f/225.0f,130.0f/255.0f,3.0f/255.0f);

    auto it_color = shader.light_uniforms.find("l_point_light_color");
    if(it_color != shader.light_uniforms.end())
        glUniform3fv(it_color->second.index,1,glm::value_ptr(light_color));

    model->draw();
    manipulation();
    /// Draw UI in the end to minimize state changes
    draw_ui();
    double time_taken = watch.read();
    //std::cout << "==== " << time_taken*1000 << " ms =====" << std::endl;
}

void mesh_update()
{
    /// Update the weights
    float* vertices(0);
    uint32_t count;
    bool update_buffer = false;
    if(uimanipulator->is_dirty && !animator->is_playing)
    {
        loader->update_weights(uimanipulator->weights,uimanipulator->weight_count);
        uimanipulator->is_dirty = false;
        update_buffer = true;
    }

    if(animator->is_playing)
    {
        update_buffer = true;
        loader->update_weights(animator->weights,24);
    }

    if(mouse_clicked || update_from_modifier)
    {
        update_buffer = true;
        loader->update_weights(modifier->modifier.w->data(),24);
    }

    if(update_buffer)
    {
        count = loader->get_vertices(&vertices);
        model->update_mesh(vertices,count,update_buffer);
    }
}

void render(GLFWwindow* window)
{
    double time = glfwGetTime();
    double delta = time - last_frame_time;
    if(delta >= 0.16)
    {
        draw();
        glfwSwapBuffers(window);
        last_frame_time = time;
        mesh_update();
    }
    else
    {
        usleep(1000);
    }
    

}

int run_gl()
{
    loader = new BlendshapeLoader();
    model = new BlendshapeModel();
    animator = new BlendshapeAnimator();
    // Initialize GLFW
    if (!glfwInit())
    {
        // Handle initialization failure
        printf("GLFW cannot be initialized \n");
        return -1;
    }
    
    // OpenGL 4.1 context is not creating the window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "My Title", NULL, NULL);
   
    // Check if window is built otherwise terminate
    if(!window)
    {
        printf("Required window cannot be created \n");
        glfwTerminate();
        return -1;
    }
    // Make GL context current to the window
    glfwMakeContextCurrent(window);
    
    // Load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        printf("Failed to initialize GLAD \n");
        return -1;
    }

    if (GLAD_GL_VERSION_4_1)
    {
        printf("Using OpenGL 4.1 version \n");
    }

    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetDropCallback(window,drop_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    /// Setup UI
    setup_imgui(window);

    glfwGetFramebufferSize(window,&frame_width,&frame_height);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
	
    glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"

    /// Load blendshape
    loader->load("../res/high-res/");
    model->load_mesh(loader->neutral_shape_path);

    modifier = new BlendshapeModifier(*loader);

    animator->read_animation("../res/animation.txt");

    uimanipulator = new UIManipulator();
    uimanipulator->setup(loader->expressions);
    
    /// Load shader
    shader = render_engine::glprogram::create(render_engine::shaders::blinn_phong_vert,
                                              render_engine::shaders::blinn_phong_frag,
                                              false);
    debug_shader = render_engine::glprogram::create(render_engine::shaders::debug_vert,
                                              render_engine::shaders::debug_frag,
                                              false);

    // While user doen't press the close button do not exit
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.1,0.1,0.1,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render(window);
    }

    shutdown_ui();
    render_engine::glprogram::destroy(shader);
    delete(loader);
    delete(model);
    delete(uimanipulator);
    // Destroy the window
    glfwDestroyWindow(window);
    
    // For terminating the window
    glfwTerminate();
    return 0;
}

#include <Eigen/Dense>
typedef Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic> CMatrixXXf;

void eigen_test()
{
    CMatrixXXf m(3,24);
    CMatrixXXf K(24,24);
    K.setIdentity();
    CMatrixXXf ka = 0.1f * K;
    CMatrixXXf ku = 0.001f * K; 
    CMatrixXXf A(51,24);

    A << m , 0.1f* K,0.0f * K ;
}

int main()
{
    //eigen_test();
    run_gl();
    return 0;
}
