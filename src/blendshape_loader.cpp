
#include "blendshape_loader.h"

#include <iostream>
#include "tinydir.h"
#include "mesh_utility.h"

BlendshapeLoader::~BlendshapeLoader(){
    if(blendshapes)
        delete(blendshapes);
    if(neutral_shape)
        delete(neutral_shape);
    if(final_shape)
        delete(final_shape);
    if(weights)
        delete(weights);
}

static std::vector<std::string> get_files(const char* dir_path)
{
    std::vector<std::string> files;

    /// Traverse directory
    tinydir_dir dir;
    tinydir_open(&dir,dir_path);

    while(dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir,&file);
        if(!file.is_dir)
        {
            if(strcmp(file.extension,"obj") == 0)
                files.push_back(std::string(file.path));
        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);

    for(int i=0;i<files.size();i++)
    {
        printf("%s \n",files[i].c_str());
    }

    return files;
}

template <int M,int N>
static void push_vertices(const float* ai_vertices,Eigen::Matrix<float,M,N>* base,int column,int count)
{
    for(int i=0;i<count;i++)
    {
        (*base)(i,column) = ai_vertices[i];
    }
}

void BlendshapeLoader::load(const char* path)
{
    std::vector<std::string> files = get_files(path);
    std::sort(files.begin(),files.end(),[](std::string a,std::string b){return a<b;});

    if(files.size() <= 0)
    {
        std::cout << "Error : No files exists !! \n" << std::endl;
        return;
    }

    bool memory_allocated = false;

    uint32_t blendshapes_count = files.size() - 1;
    uint32_t last_vertex_count = 0;
    uint32_t blendshape_index = 0;

    MeshUtility utility;

    for(int i=0;i<files.size();i++)
    {
        std::vector<float> mesh_positions; //vertex positions
        if(!utility.load_position(files[i],mesh_positions))
        {
            std::cout << "Error : Unable to read file !! \n" << std::endl;
            return;
        }

        vertex_count = mesh_positions.size()/3.0f;
        int row_count = mesh_positions.size();

        last_vertex_count = last_vertex_count == 0 ? vertex_count : last_vertex_count;
        if(vertex_count <= 0 || last_vertex_count != vertex_count)
        {
            std::cout << "Error : Vertex count is less than zero or not matching !! \n" << std::endl;
            return;
        }

        if(!memory_allocated)
        {
            /// Dropping neutral
            /// XYZ in column
            blendshapes = new MatrixXXf(row_count,blendshapes_count);
            neutral_shape = new VectorXf(row_count);
            final_shape = new VectorXf(row_count);
            weights = new VectorXf(blendshapes_count);
            memory_allocated = true;
        }
        
        int last_dot = files[i].find_last_of('.');
        int last_div = files[i].find_last_of('/') + 1;
        std::string expression_name = files[i].substr(last_div,last_dot-last_div);
        
        /// Keep neutral 
        if(expression_name.compare("neutral") == 0)
        {
            push_vertices<Eigen::Dynamic,1> (mesh_positions.data(),neutral_shape,0,row_count);
            neutral_shape_path = files[i];
        }
        else
        {
            /// Push expressions name
            expressions.push_back(expression_name);
            push_vertices<Eigen::Dynamic,Eigen::Dynamic> (mesh_positions.data(),blendshapes,blendshape_index,row_count);
            blendshape_index++;
        }
    }

    /// Calculating deltashapes
    *blendshapes = ((*blendshapes)).colwise() - *neutral_shape;

    /// Initialize weights; 
    *weights = weights->Zero(blendshapes_count);

    /// Calculate final shape to initialze
    (*final_shape) = (*neutral_shape) + ((*blendshapes) * (*weights));
}

uint32_t BlendshapeLoader::get_vertices(float** vertices)
{
    *vertices = final_shape->data();
    return final_shape->size();
}

void BlendshapeLoader::update_weights(float* in_weights,uint32_t count)
{
    for(int i=0;i<count;i++)
    {
        (*weights)(i,0) = in_weights[i];
    }
    
    (*final_shape) = (*neutral_shape) + ((*blendshapes) * (*weights));
}
