#ifndef __BVH_H__
#define __BVH_H__

#include <glm/glm.hpp>
#include <iostream>
#include "glprogram.h"

class BlendShapeMesh;

class Ray
{
public:
    glm::vec3 position;
    glm::vec3 direction;
    void update(const glm::mat4& proj,const glm::mat4& view,
                const glm::vec3& camera_pos,float pos_x,
                float pos_y,int width,int height)
    {
        glm::vec4 ray((2.0f * pos_x) / width - 1.0f, 
                           1.0f - (2.0f * pos_y) / height,
                           -1.0f,
                           1.0f);
        ray = glm::inverse(proj) * ray;
        ray = glm::vec4(ray.x,ray.y, -1.0, 0.0);
        ray = glm::inverse(view) * ray;
        direction = glm::normalize(glm::vec3(ray.x,ray.y,ray.z));
        position = glm::vec3(camera_pos);
    }
};

class IntersectionFinder
{
public:
    static bool ray_triangle(const glm::vec3& rayOrigin, 
                           const glm::vec3& rayVector, 
                           const float* vert,
                           glm::vec3& outIntersectionPoint,
                           float& outT);

    static void barycentric(const glm::vec3& p,const glm::vec3& a,const glm::vec3& b,
                 const glm::vec3& c, float &u, float &v, float &w);
    static void triangle_vertices(const BlendShapeMesh& mesh,
                              float* vertices,int triangle_index,
                              int& v1_index,int& v2_index,int& v3_index);
    static int get_intersection(const Ray& ray,const glm::mat4& proj_view,
                                const BlendShapeMesh& mesh,
                                glm::vec3& nearest_vec,
                                int& vertex_index);
};

#endif