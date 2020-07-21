#ifndef __BLENDSHAPE_MESH_H__
#define __BLENDSHAPE_MESH_H__

#include "inttypes.h"
#include <string>

class BlendShapeMesh
{
public:
    float* positions;
    float* normals;
    float* uvs;

    uint32_t* pindices;
    uint32_t pindices_count;

    uint32_t* indices;
    uint32_t indices_count;

    float* transit_buffer;

    BlendShapeMesh();
    ~BlendShapeMesh();
private:
	BlendShapeMesh(const BlendShapeMesh&);
	BlendShapeMesh& operator=(const BlendShapeMesh&);
};

class BlendshapeModel
{   
private:
    uint32_t VBO,VNO,IBO,VAO;
public:
    BlendShapeMesh mesh;
    BlendshapeModel();
    ~BlendshapeModel();
    void load_mesh(const std::string& file);
    void update_mesh(const float* vertices,uint32_t vertex_count,bool update_buffer);
    void draw();
};

#endif
