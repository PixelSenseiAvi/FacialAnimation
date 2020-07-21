#ifndef __MESH_UTILITY_H__
#define __MESH_UTILITY_H__

#include "inttypes.h"

#include <string>
#include <vector>

class BlendShapeMesh;

struct Vertex
{
	float px, py, pz;
	float nx, ny, nz;
	float tx, ty;
	/// for obj vertex indices
	uint32_t pindex;
};

class MeshUtility
{
public:
    enum
    {
        POSITION_INDICES,
        NORMAL_INDICES,
        UV_INDICES
    };
	bool load_obj(const std::string& filename);
    bool load_blendshape_obj(const std::string& filename,BlendShapeMesh& mesh);
	bool load_position(const std::string& filename,std::vector<float>& positions);
    bool load_normal(const std::string& filename,std::vector<float>& normals);
    bool load_tex(const std::string& filename,std::vector<float>& tex_coords);
    bool load_indices(const std::string& filename,std::vector<uint32_t>& indices,int type);

	/// This function assumes that the out vector is preallocated
	void convert_indices(float* out_data,float* in_data,const uint32_t* indices,
						 int stride,int indices_count);
    
    void optimize_mesh(std::vector<Vertex>& in_vertices,std::vector<uint32_t>& out_indices);
};

#endif