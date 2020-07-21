#ifndef __SPHERE_PRIMITIVE_H__
#define __SPHERE_PRIMITIVE_H__

#include <inttypes.h>
#include <glm/glm.hpp>
#include "glprogram.h"

class Primitives
{
    enum 
    {
        SPHERE,
        CUBE,
        LINE,
        TRIANGLE
    };

    uint32_t VAO,VBO,IBO;
    uint32_t indices_count;
public:
    Primitives():VAO(0),VBO(0),IBO(0){}
    ~Primitives();
    void create();
    void draw();
    static void draw_cube(const glm::mat4& proj_view,const glm::vec3& pos,
                          const render_engine::GLProgram& shader);
    static void draw_debug_line(const glm::mat4& proj_view,
                                const glm::vec3& from,
                                const glm::vec3& to,
                                const render_engine::GLProgram& shader);
    static void draw_debug_face(const glm::mat4& proj_view,
                                const glm::mat4& model,
                                const float* points,
                                const render_engine::GLProgram& shader);
};

#endif