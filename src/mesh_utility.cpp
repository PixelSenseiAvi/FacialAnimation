#include "mesh_utility.h"
#include <meshoptimizer.h>

#include <iostream>
#include "blendshape_model.h"

bool MeshUtility::load_position(const std::string& filename,std::vector<float>& positions)
{
    char  s[200];
	float  x, y, z;

	// open file (in ASCII mode)
	FILE* in = fopen(filename.c_str(), "r");
	if (!in) return false;

	// clear line once
	memset(&s, 0, 200);

	// parse line by line (currently only supports vertex positions & faces
	while (in && !feof(in) && fgets(s, 200, in))
	{
		// comment
		if (s[0] == '#' || isspace(s[0])) continue;

		// vertex
		else if (strncmp(s, "v ", 2) == 0)
		{
			if (sscanf(s, "v %f %f %f", &x, &y, &z))
			{
                positions.push_back(x);
                positions.push_back(y);
                positions.push_back(z);
			}
		}
		// clear line
		memset(&s, 0, 200);
	}

	fclose(in);
	return true;
}

bool MeshUtility::load_normal(const std::string& filename,std::vector<float>& normals)
{
    char  s[200];
	float  x, y, z;

	// open file (in ASCII mode)
	FILE* in = fopen(filename.c_str(), "r");
	if (!in) return false;

	// clear line once
	memset(&s, 0, 200);

	// parse line by line (currently only supports vertex positions & faces
	while (in && !feof(in) && fgets(s, 200, in))
	{
		// comment
		if (s[0] == '#' || isspace(s[0])) continue;

		// normal
		else if (strncmp(s, "vn ", 3) == 0)
		{
			if (sscanf(s, "vn %f %f %f", &x, &y, &z))
			{
                normals.push_back(x);
                normals.push_back(y);
                normals.push_back(z);
			}
		}
		// clear line
		memset(&s, 0, 200);
	}

	fclose(in);
	return true;
}

bool MeshUtility::load_tex(const std::string& filename,std::vector<float>& tex_coords)
{
    char  s[200];
	float  x, y, z;

	// open file (in ASCII mode)
	FILE* in = fopen(filename.c_str(), "r");
	if (!in) return false;

	// clear line once
	memset(&s, 0, 200);

	// parse line by line (currently only supports vertex positions & faces
	while (in && !feof(in) && fgets(s, 200, in))
	{
		// comment
		if (s[0] == '#' || isspace(s[0])) continue;

		// texture coordinate
		else if (strncmp(s, "vt ", 3) == 0)
		{
			if (sscanf(s, "vt %f %f", &x, &y))
			{
                tex_coords.push_back(x);
                tex_coords.push_back(y);
			}
		}
		// clear line
		memset(&s, 0, 200);
	}

	fclose(in);
	return true;
}

bool MeshUtility::load_indices(const std::string& filename,std::vector<uint32_t>& indices,int type)
{
    char  s[200];
	float  x, y, z;

	// open file (in ASCII mode)
	FILE* in = fopen(filename.c_str(), "r");
	if (!in) return false;

	// clear line once
	memset(&s, 0, 200);

	// parse line by line (currently only supports vertex positions & faces
	while (in && !feof(in) && fgets(s, 200, in))
	{
		// comment
		if (s[0] == '#' || isspace(s[0])) continue;

		// face
		else if (strncmp(s, "f ", 2) == 0)
		{
			int component(0), nV(0);
			bool endOfVertex(false);
			char *p0, *p1(s + 1);

			std::vector<int> polygon;

			// skip white-spaces
			while (*p1 == ' ') ++p1;

			while (p1)
			{
				p0 = p1;

				// overwrite next separator

				// skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
				while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && *p1 != ' ' && *p1 != '\0') ++p1;

				// detect end of vertex
				if (*p1 != '/')
				{
					endOfVertex = true;
				}

				// replace separator by '\0'
				if (*p1 != '\0')
				{
					*p1 = '\0';
					p1++; // point to next token
				}

				// detect end of line and break
				if (*p1 == '\0' || *p1 == '\n')
				{
					p1 = 0;
				}

				// read next vertex component
				if (*p0 != '\0')
				{
                    if(type == MeshUtility::POSITION_INDICES && component == 0)
                    {
                        indices.push_back(atoi(p0) - 1);
                    }
                    else if(type == MeshUtility::UV_INDICES && component == 1)
                    {
                        indices.push_back(atoi(p0) - 1);
                    }
                    else if(type == MeshUtility::NORMAL_INDICES && component == 2)
                    {
                        indices.push_back(atoi(p0) - 1);
                    }
				}

				++component;

				if (endOfVertex)
				{
					component = 0;
					nV++;
					endOfVertex = false;
				}
			}
		}
		// clear line
		memset(&s, 0, 200);
	}

	fclose(in);
	return true;
}

void MeshUtility::convert_indices(float* out_data,float* in_data,const uint32_t* indices,
                                  int stride,int indices_count)
{
    for(int i=0;i<indices_count;i++)
    {
        int in_index = indices[i] * stride;
        int out_index = i * stride;
        for(int j=0;j<stride;j++)
        {
            out_data[out_index + j] = in_data[in_index + j];
        }
    }
}

void MeshUtility::optimize_mesh(std::vector<Vertex>& in_vertices,std::vector<uint32_t>& out_indices)
{	
	int total_indices = in_vertices.size();
	std::cout << "indices count " << total_indices << std::endl;

	std::vector<uint32_t> remap(total_indices);
	
	/// Check for binary equivalent need positions,normals,uvs in the same struct
	/// Generate indices and remove redundant vertices
	size_t total_vertices = meshopt_generateVertexRemap(&remap[0],NULL,total_indices,&in_vertices[0],total_indices,sizeof(Vertex));
	std::cout << "optimized vertices count " << total_vertices << std::endl;

	std::vector<uint32_t> temp_indices(total_indices);
	/// Source & destination needs to different
	/// destination[i] = remap[source[i]]
	meshopt_remapIndexBuffer(&temp_indices[0],NULL,total_indices,&remap[0]);
	
	std::vector<Vertex> out_vertices(total_vertices);
	/// Internal check for same source and destination
	/// TODO remap the original index buffer as well
	/// destination[map[i]] = source[i]
	meshopt_remapVertexBuffer(&out_vertices[0],&in_vertices[0],total_indices,sizeof(Vertex),&remap[0]);

	/// Optimize vertex cache by changing indices
	/// Same source & destination works
	meshopt_optimizeVertexCache(&temp_indices[0],&temp_indices[0],total_indices,total_vertices);

	/// Optimize vertex fetch by remapping index buffer
	std::vector<uint32_t> fetch_remap(total_vertices);
	meshopt_optimizeVertexFetchRemap(&fetch_remap[0],&temp_indices[0],total_indices,total_vertices);
	meshopt_remapVertexBuffer(&out_vertices[0],&out_vertices[0],total_vertices,sizeof(Vertex),&fetch_remap[0]);
	
//    out_indices.resize(total_indices);
	meshopt_remapIndexBuffer(&out_indices[0],&temp_indices[0],total_indices,&fetch_remap[0]);

	///Copy everything back to in_vertices
	in_vertices.resize(total_vertices);
	memcpy(&in_vertices[0],&out_vertices[0],total_vertices * sizeof(Vertex));
}

bool MeshUtility::load_blendshape_obj(const std::string& filename,BlendShapeMesh& mesh)
{
	std::vector<float> obj_positions;
	std::vector<float> obj_normals;
	std::vector<float> obj_uvs;
	std::vector<uint32_t> obj_pindices;
	std::vector<uint32_t> obj_nindices;
	std::vector<uint32_t> obj_tindices;

	load_position(filename,obj_positions);
    load_normal(filename,obj_normals);
	load_tex(filename,obj_uvs);

	int temp = obj_positions.size();
	std::cout << "position count " << temp << std::endl;
	temp = obj_normals.size();
	std::cout << "normal count " << temp << std::endl;
	temp = obj_uvs.size();
	std::cout << "uv count " << temp << std::endl;

    load_indices(filename,obj_pindices,MeshUtility::POSITION_INDICES);
    load_indices(filename,obj_nindices,MeshUtility::NORMAL_INDICES);
	load_indices(filename,obj_tindices,MeshUtility::UV_INDICES);

	int pindices_count = obj_pindices.size();
	bool has_normal = obj_normals.size() > 0 ? true : false;
	bool has_uv = obj_uvs.size() > 0 ? true : false;
	
	std::vector<Vertex> vertices(pindices_count);
	for (size_t i = 0; i < pindices_count; ++i)
	{
		int pindex = obj_pindices[i] * 3;
		int nindex = has_normal ? obj_nindices[i] * 3 : -1;
		int tindex = has_uv ? obj_tindices[i] * 2 : -1;

		Vertex v =
			{
		        obj_positions[pindex + 0],
		        obj_positions[pindex + 1],
		        obj_positions[pindex + 2],

		        has_normal ? obj_normals[nindex + 0] : 0,
		        has_normal ? obj_normals[nindex + 1] : 0,
		        has_normal ? obj_normals[nindex + 2] : 0,

		        has_uv  ? obj_uvs[tindex + 0] : 0,
		        has_uv  ? obj_uvs[tindex + 1] : 0,
				
				obj_pindices[i]
		    };

		vertices[i] = v;
	}

	/// Optimize mesh
	std::vector<uint32_t> indices(pindices_count);
	optimize_mesh(vertices,indices);

	/// Put everything to the mesh
	uint32_t total_indices = indices.size();
	uint32_t total_vertices = vertices.size();
	mesh.positions = new float[total_vertices * 3];
	mesh.transit_buffer = new float[total_vertices * 3];
	if(has_normal)
		mesh.normals = new float[total_vertices * 3];
	if(has_uv)
		mesh.uvs = new float[total_vertices * 2];

	mesh.pindices = new uint32_t[total_vertices];
	mesh.indices = new uint32_t[total_indices];

	mesh.pindices_count = total_vertices;
	mesh.indices_count = total_indices;

	for(uint32_t i=0;i<total_vertices;i++)
	{
		int pindex = i*3;
		int tindex = i*2;

		/// positions
		mesh.positions[pindex + 0] = vertices[i].px;
		mesh.positions[pindex + 1] = vertices[i].py;
		mesh.positions[pindex + 2] = vertices[i].pz;

		mesh.transit_buffer[pindex + 0] = vertices[i].px;
		mesh.transit_buffer[pindex + 1] = vertices[i].py;
		mesh.transit_buffer[pindex + 2] = vertices[i].pz;

		/// normals
		if(has_normal)
		{
			mesh.normals[pindex + 0] = vertices[i].nx;
			mesh.normals[pindex + 1] = vertices[i].ny;
			mesh.normals[pindex + 2] = vertices[i].nz;
		}

		/// uvs
		if(has_uv)
		{
			mesh.uvs[tindex + 0] = vertices[i].tx;
			mesh.uvs[tindex + 1] = vertices[i].ty;
		}

		/// obj index
		mesh.pindices[i] = vertices[i].pindex;
	}

	memcpy(mesh.indices,&indices[0],total_indices * sizeof(uint32_t));
    
    /// Test code for indexing
//    std::vector<float> temp_values(total_vertices * 3);
//    for(int i=0;i<total_vertices;i++)
//    {
//        int index = vertices[i].pindex * 3;
//        temp_values[i*3 + 0] = obj_positions[index + 0];
//        temp_values[i*3 + 1] = obj_positions[index + 1];
//        temp_values[i*3 + 2] = obj_positions[index + 2];;
//    }
//
//    int v = total_vertices;
    
	return true;
}
