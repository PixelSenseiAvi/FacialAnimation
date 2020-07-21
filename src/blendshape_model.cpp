#include "blendshape_model.h"

#include "glad/glad.h"
#include "teapot.h"
#include "mesh_utility.h"

#include <iostream>
#include <fstream>
#include <vector>

BlendShapeMesh::BlendShapeMesh()
    :positions(0),
    normals(0),
    uvs(0),
    pindices(0),
    pindices_count(0),
    indices(0),
    indices_count(0),
    transit_buffer(0)
{}

BlendShapeMesh::~BlendShapeMesh()
{  
    if(positions)
        delete[] positions;
    if(normals)
        delete[] normals;
    if(uvs)
        delete[] uvs;
    if(pindices)
        delete[] pindices;
    if(indices)
        delete[] indices;
    if(transit_buffer)
        delete[] transit_buffer;
    
    indices_count = 0;
    pindices_count = 0;
}


BlendshapeModel::BlendshapeModel()
{
    VAO = 0;
    VBO = 0;
    VNO = 0;
    IBO = 0;
}

BlendshapeModel::~BlendshapeModel()
{
    if(VAO != 0)
        glDeleteVertexArrays(1,&VAO);
    if(VBO != 0)
        glDeleteBuffers(1,&VBO);
    if(VNO != 0)
        glDeleteBuffers(1,&VNO);
    if(IBO != 0)
        glDeleteBuffers(1,&IBO);
}

void BlendshapeModel::load_mesh(const std::string& file)
{
    /// Feed Buffer data
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&VNO);
    glGenBuffers(1,&IBO);

    MeshUtility utility;
    utility.load_blendshape_obj(file,mesh);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,mesh.pindices_count * 3 * sizeof(float),mesh.positions,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,VNO);
    glBufferData(GL_ARRAY_BUFFER,mesh.pindices_count * 3 * sizeof(float),mesh.normals,GL_STATIC_DRAW);

    /// Setup vertex array
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,VNO);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,mesh.indices_count * sizeof(uint32_t),mesh.indices,GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

}

void BlendshapeModel::update_mesh(const float* vertices,uint32_t vertex_count,bool update_buffer)
{
    if(update_buffer)
    {
        for(int i=0;i<mesh.pindices_count;i++)
        {
            int tindex = i * 3;
            int vindex = mesh.pindices[i] * 3;
        
            if(mesh.pindices[i] < 0)
            {
                std::cout << "Serious error " << mesh.pindices[i] << std::endl;
                return;
            }
         
            mesh.transit_buffer[tindex + 0] = vertices[vindex + 0];
            mesh.transit_buffer[tindex + 1] = vertices[vindex + 1];
            mesh.transit_buffer[tindex + 2] = vertices[vindex + 2];
        }
    }


     glBindBuffer(GL_ARRAY_BUFFER,VBO);
     glBufferData(GL_ARRAY_BUFFER, mesh.pindices_count * 3 * sizeof(float), 0, GL_DYNAMIC_DRAW);
     //glBufferSubData(GL_ARRAY_BUFFER,0,3 * vertex_count * sizeof(float),vertices);
     void* ptr = glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
     memcpy(ptr,mesh.transit_buffer,mesh.pindices_count * 3 * sizeof(float));
     glUnmapBuffer(GL_ARRAY_BUFFER);
     glBindBuffer(GL_ARRAY_BUFFER,0);
}

void BlendshapeModel::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,mesh.indices_count,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}
