#ifndef __GLPROGRAM_H__
#define __GLPROGRAM_H__

#include <inttypes.h>
#include <string>
#include <unordered_map>


namespace render_engine
{
    namespace shaders
    {
        extern const char* base;
        
        extern const char* blinn_phong_vert;
        extern const char* blinn_phong_frag;
        extern const char* debug_vert;
        extern const char* debug_frag;
    }
    struct VertexAttrib
    {
        uint32_t index;
        int32_t size;
        uint32_t type;
        std::string name;
    };

    struct Uniform
    {
        uint32_t index;
        int32_t size;
        uint32_t type;
        std::string name;
    };

    struct GLProgram
    {
        enum
        {
            ATTRIB_POSITION = 0,
            ATTRIB_NORMAL,
            ATTRIB_TANGENT,
            ATTRIB_TEX_COORD,
            ATTRIB_TEX_COORD1,
            ATTRIB_TEX_COORD2,
            ATTRIB_TEX_COORD3,
            ATTRIB_COLOR,
            ATTRIB_MAX
        };
        enum 
        {
            UNIFORM_MVP = 0,
            UNIFORM_M,
            UNIFORM_V,
            UNIFORM_P,
            UNIFORM_CAMERA_POSITION,
            UNIFORM_MAX
        };

        static const char* UNIFORM_NAME_MVP;
        static const char* UNIFORM_NAME_M;
        static const char* UNIFORM_NAME_V;
        static const char* UNIFORM_NAME_P;
        static const char* UNIFORM_NAME_CAMERA_POSITION;

        static const char* ATTRIB_NAME_POSITION;
        static const char* ATTRIB_NAME_COLOR;
        static const char* ATTRIB_NAME_NORMAL;
        static const char* ATTRIB_NAME_TANGENT;
        static const char* ATTRIB_NAME_TEX_COORD;
        static const char* ATTRIB_NAME_TEX_COORD1;
        static const char* ATTRIB_NAME_TEX_COORD2;
        static const char* ATTRIB_NAME_TEX_COORD3;

        uint32_t program;
        uint32_t vert_shader;
        uint32_t frag_shader;
        int32_t uniforms[UNIFORM_MAX];
        std::unordered_map<std::string,VertexAttrib> attribs;
        std::unordered_map<std::string,Uniform> custom_uniforms;
        std::unordered_map<std::string,Uniform> light_uniforms;
    };

    namespace glprogram
    {
        GLProgram create(const char* vert_path,const char* frag_path,bool include_base);
        bool compile_shader(uint32_t* shader,uint32_t type,const std::string& source,const std::string& headers,const std::string& defines);
        bool link(GLProgram& gl_program);
        void use(uint32_t prog_id);
        void update_uniforms(GLProgram& gl_program);
        void bind_predefined_vertex_attribs(GLProgram& gl_program);
        void parse_vertex_attribs(GLProgram& gl_program);
        void parse_uniforms(GLProgram& gl_program);
        std::string log_shader(uint32_t shader);
        void clear_shader(GLProgram& gl_program);
        void destroy(GLProgram& gl_program);
        /// Don't know if we need shader access here for now
    }
}

#endif