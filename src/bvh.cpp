#include "bvh.h"
#include "blendshape_model.h"
#include "primitives.h"
#include <limits>

bool IntersectionFinder::ray_triangle(const glm::vec3& rayOrigin, 
                       const glm::vec3& rayVector, 
                       const float* vert,
                       glm::vec3& outIntersectionPoint,
                       float& outT)
{
    const float EPSILON = 0.0000001;
    glm::vec3 vertex0 = glm::vec3(vert[0],vert[1],vert[2]);
    glm::vec3 vertex1 = glm::vec3(vert[3],vert[4],vert[5]); 
    glm::vec3 vertex2 = glm::vec3(vert[6],vert[7],vert[8]);
    glm::vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = glm::cross(rayVector,edge2);;
    a = glm::dot(edge1,h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = rayOrigin - vertex0;
    u = f * (glm::dot(s,h));
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s,edge1);
    v = f * glm::dot(rayVector,q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2,q);
    if (t > EPSILON) // ray intersection
    {
        outT = t;
        outIntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

void IntersectionFinder::barycentric(const glm::vec3& p,const glm::vec3& a,const glm::vec3& b,
                 const glm::vec3& c, float &u, float &v, float &w)
{
    glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}

int IntersectionFinder::get_intersection(const Ray& ray,const glm::mat4& proj_view,
                                         const BlendShapeMesh& mesh,
                                         glm::vec3& nearest_vec,
                                         int& vertex_index)
{
    int indices_count = mesh.indices_count;

    glm::vec3 final_intersect_point(0.0f);
    glm::vec3 intersect_point(0.0f);
    float min_t = std::numeric_limits<float>::max();
    float t(0);
    int index = -1;
    float vertices[9];
    int v0_index,v1_index,v2_index;
    for(int i=0;i<indices_count;i+=3)
    {
        triangle_vertices(mesh,vertices,i,v0_index,v1_index,v2_index);
        bool intersects = ray_triangle(ray.position,ray.direction,vertices,
                                                intersect_point,t);
        if(intersects && t < min_t)
        {
            min_t = t;
            index = i;
            final_intersect_point = intersect_point;
        }
    }
    
    if(index != -1)
    {
        triangle_vertices(mesh,vertices,index,v0_index,v1_index,v2_index);
        glm::vec3 vertex0 = glm::vec3(vertices[0],vertices[1],vertices[2]);
        glm::vec3 vertex1 = glm::vec3(vertices[3],vertices[4],vertices[5]); 
        glm::vec3 vertex2 = glm::vec3(vertices[6],vertices[7],vertices[8]);

        float u(0),v(0),w(0);
        barycentric(final_intersect_point,vertex0,vertex1,vertex2,u,v,w);
        nearest_vec = glm::vec3(0.0f);
        if(u > v)
        {
            if(u > w)
            {
                nearest_vec = vertex0;
                vertex_index = v0_index;
            }
            else
            {
                nearest_vec = vertex2;
                vertex_index = v2_index;
            }
        }
        else
        {
            if(v > w)
            {
                nearest_vec = vertex1;
                vertex_index = v1_index;
            }
            else
            {
                nearest_vec = vertex2;
                vertex_index = v2_index;
            }
        }
    }
    return index;
}

void IntersectionFinder::triangle_vertices(const BlendShapeMesh& mesh,
                                           float* vertices,
                                           int triangle_index,
                                           int& v1_index,int& v2_index,
                                           int& v3_index)
{
    v1_index = mesh.indices[triangle_index] * 3;
    v2_index = mesh.indices[triangle_index+1] * 3;
    v3_index = mesh.indices[triangle_index+2] * 3;

    vertices[0] = mesh.transit_buffer[v1_index];
    vertices[1] = mesh.transit_buffer[v1_index + 1];
    vertices[2] = mesh.transit_buffer[v1_index + 2];
    vertices[3] = mesh.transit_buffer[v2_index];
    vertices[4] = mesh.transit_buffer[v2_index + 1];
    vertices[5] = mesh.transit_buffer[v2_index + 2];
    vertices[6] = mesh.transit_buffer[v3_index];
    vertices[7] = mesh.transit_buffer[v3_index + 1];
    vertices[8] = mesh.transit_buffer[v3_index + 2];
}