#include "primitives.h"

#include "glad/glad.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Primitives::~Primitives()
{
    if(VAO != 0)
        glDeleteVertexArrays(1,&VAO);
    if(VBO != 0)
        glDeleteBuffers(1,&VBO);
    if(IBO != 0)
        glDeleteBuffers(1,&IBO);
}

void Primitives::create()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }
    bool oddRow = false;
    for (int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y       * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y       * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indices_count = indices.size();
    std::vector<float> data;
    for (int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
    }
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    float stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glBindVertexArray(0);
}

void Primitives::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,indices_count,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

void Primitives::draw_cube(const glm::mat4& proj_view,const glm::vec3& pos,
                           const render_engine::GLProgram& shader)
{
    static GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    static uint32_t VAO(0);
    static uint32_t VBO(0);
    if(VAO == 0)
    {
        glGenBuffers(1,&VBO);
        glGenVertexArrays(1,&VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, 36*3*sizeof(float), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    render_engine::glprogram::use(shader.program);
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f),pos);
    model_matrix = glm::scale(model_matrix,glm::vec3(0.3f));
    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_MVP], 1, GL_FALSE, glm::value_ptr(proj_view * model_matrix));
    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_M], 1, GL_FALSE, glm::value_ptr(model_matrix));

    glBindVertexArray( VAO );
    glDrawArrays(GL_TRIANGLES, 0, 12*3);
    glBindVertexArray(0);
    render_engine::glprogram::use(0);
}

void Primitives::draw_debug_line(const glm::mat4& proj_view,
                                 const glm::vec3& from,
                                 const glm::vec3& to,
                                 const render_engine::GLProgram& shader)
{
    struct LineSegment_t
    {
        float x1, y1,z1;
        float x2, y2,z2;
    };

    LineSegment_t line
    {
        from.x,
        from.y,
        from.z,
        to.x,
        to.y,
        to.z
    };

    static uint32_t VAO(0);
    static uint32_t VBO(0);
    if(VAO == 0)
    {
        glGenBuffers(1,&VBO);
        glGenVertexArrays(1,&VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    render_engine::glprogram::use(shader.program);
    glm::mat4 model_matrix(1.0f);
    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_MVP], 1, GL_FALSE, glm::value_ptr(proj_view));
    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_M], 1, GL_FALSE, glm::value_ptr(model_matrix));

    glBindVertexArray( VAO );
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LineSegment_t), &line, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    render_engine::glprogram::use(0);
}

void Primitives::draw_debug_face(const glm::mat4& proj_view,
                                 const glm::mat4& model,
                                 const float* points,
                                 const render_engine::GLProgram& shader)
{
    static uint32_t VAO(0);
    static uint32_t VBO(0);
    if(VAO == 0)
    {
        glGenBuffers(1,&VBO);
        glGenVertexArrays(1,&VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    render_engine::glprogram::use(shader.program);
    glm::mat4 model_matrix(1.0f);
    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_MVP], 1, GL_FALSE, glm::value_ptr(proj_view));
    glUniformMatrix4fv (shader.uniforms[render_engine::GLProgram::UNIFORM_M], 1, GL_FALSE, glm::value_ptr(model_matrix));

    glDisable (GL_DEPTH_TEST); 

    glBindVertexArray( VAO );
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    render_engine::glprogram::use(0);

    glEnable (GL_DEPTH_TEST); 

}