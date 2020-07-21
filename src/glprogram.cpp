#include "glprogram.h"
#include "glad/glad.h"

#include "file_io.h"

namespace render_engine
{
    namespace shaders
    {
        const char* base = "";
        const char* blinn_phong_vert = "../res/shaders/blinn.vert";
        const char* blinn_phong_frag = "../res/shaders/blinn.frag";

        const char* debug_vert = "../res/shaders/debug.vert";
        const char* debug_frag = "../res/shaders/debug.frag"; 
    }

    const char* GLProgram::UNIFORM_NAME_MVP = "u_mvp_matrix";
    const char* GLProgram::UNIFORM_NAME_M = "u_m_matrix";
    const char* GLProgram::UNIFORM_NAME_V = "u_v_matrix";
    const char* GLProgram::UNIFORM_NAME_P = "u_p_matrix";
    const char* GLProgram::UNIFORM_NAME_CAMERA_POSITION = "u_camera_pos";

    const char* GLProgram::ATTRIB_NAME_POSITION = "a_position";
    const char* GLProgram::ATTRIB_NAME_COLOR = "a_color";
    const char* GLProgram::ATTRIB_NAME_NORMAL = "a_normal";
    const char* GLProgram::ATTRIB_NAME_TANGENT = "a_tangent";
    const char* GLProgram::ATTRIB_NAME_TEX_COORD = "a_uv0";
    const char* GLProgram::ATTRIB_NAME_TEX_COORD1 = "a_uv1";
    const char* GLProgram::ATTRIB_NAME_TEX_COORD2 = "a_uv2";
    const char* GLProgram::ATTRIB_NAME_TEX_COORD3 = "a_uv3";
    
    namespace glprogram
    {
        GLProgram create(const char* vert_path,const char* frag_path,bool include_base)
        {
            std::string vertex_source = file_io::read_text(vert_path);
            std::string frag_source = file_io::read_text(frag_path);
            
            std::string header = include_base ? file_io::read_text(shaders::base) : "";
            std::string defines_vertex = include_base ? "#define VERTEX_STAGE 1 \n #define FRAGMENT_STAGE 0 \n" : "";
            std::string defines_fragment = include_base ? "#define VERTEX_STAGE 0 \n #define FRAGMENT_STAGE 1 \n" : "";

            GLProgram gl_program;
            gl_program.program = glCreateProgram();
            
            gl_program.vert_shader = 0;
            gl_program.frag_shader = 0;


            if(!compile_shader(&gl_program.vert_shader,GL_VERTEX_SHADER,vertex_source,header,defines_vertex.c_str()))
            {
                printf("Error : Failed to compile vertex shader");
                destroy(gl_program);
                return gl_program;
            }

            if(!compile_shader(&gl_program.frag_shader,GL_FRAGMENT_SHADER,frag_source,header,defines_fragment.c_str()))
            {
                printf("Error : Failed to compile fragment shader");
                destroy(gl_program);
                return gl_program;
            }

            if(gl_program.vert_shader)
            {
                glAttachShader(gl_program.program,gl_program.vert_shader);
            }

            if(gl_program.frag_shader)
            {
                glAttachShader(gl_program.program,gl_program.frag_shader);
            }

            if(!link(gl_program))
            {
                return gl_program;
            }

            update_uniforms(gl_program);
            parse_uniforms(gl_program);
            


            return gl_program;

        }

        bool compile_shader(uint32_t* shader,uint32_t type,const std::string& source,const std::string& headers,const std::string& defines)
        {
            GLint status = GL_TRUE;

            const char* predefines = 
            "#version 410 \n";

            
            const char* sources[] = 
            {
                predefines,
                defines.c_str(),
                headers.c_str(),
                source.c_str()
            };

            *shader = glCreateShader(type);
            glShaderSource(*shader,sizeof(sources)/sizeof(*sources),sources,nullptr);
            glCompileShader(*shader);

            glGetShaderiv(*shader,GL_COMPILE_STATUS,&status);

            if(!status)
            {
                GLsizei length;
                glGetShaderiv(*shader,GL_SHADER_SOURCE_LENGTH,&length);
                GLchar* src = (GLchar*) malloc (sizeof(GLchar)* length);

                glGetShaderSource(*shader,length,nullptr,src);
                printf("Error : unable to compile shader : \n%s\n",src);

                printf("Error : error thrown while compiling : \n%s\n",log_shader(*shader).c_str());

                free(src);

            }

            return status == GL_TRUE;

        }
        
        bool link(GLProgram& gl_program)
        {
            bind_predefined_vertex_attribs(gl_program);
            glLinkProgram(gl_program.program);

            GLint status = GL_TRUE;
            glGetProgramiv(gl_program.program,GL_LINK_STATUS,&status);

            if(status == GL_FALSE)
            {
                glDeleteProgram(gl_program.program);
                gl_program.program = 0;
                printf("Error : Failed to link program");
            }
            else
            {
                parse_vertex_attribs(gl_program);
                
            }

            clear_shader(gl_program);

            return (status == GL_TRUE);
        }
        
        void use(uint32_t prog_id)
        {
            glUseProgram(prog_id);
        }

        void update_uniforms(GLProgram& gl_program)
        {
            gl_program.uniforms[GLProgram::UNIFORM_MVP] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_MVP);
            gl_program.uniforms[GLProgram::UNIFORM_M] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_M);
            gl_program.uniforms[GLProgram::UNIFORM_V] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_V);
            gl_program.uniforms[GLProgram::UNIFORM_P] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_P);
            gl_program.uniforms[GLProgram::UNIFORM_CAMERA_POSITION] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_CAMERA_POSITION);
            // gl_program.uniforms[GLProgram::UNIFORM_SAMPLER0] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_SAMPLER0);
            // gl_program.uniforms[GLProgram::UNIFORM_SAMPLER1] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_SAMPLER1);
            // gl_program.uniforms[GLProgram::UNIFORM_SAMPLER2] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_SAMPLER2);
            // gl_program.uniforms[GLProgram::UNIFORM_SAMPLER3] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_SAMPLER3);
            // gl_program.uniforms[GLProgram::UNIFORM_SAMPLER4] = glGetUniformLocation(gl_program.program,GLProgram::UNIFORM_NAME_SAMPLER4);

            // use(gl_program);
            // if(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER0] != -1)
            //     glUniform1i(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER0],0);

            // if(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER1] != -1)
            //     glUniform1i(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER1],1);

            // if(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER2] != -1)
            //     glUniform1i(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER2],2);

            // if(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER3] != -1)
            //     glUniform1i(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER3],3);

            // if(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER4] != -1)
            //     glUniform1i(gl_program.uniforms[GLProgram::UNIFORM_SAMPLER4],4);
        }
        
        void bind_predefined_vertex_attribs(GLProgram& gl_program)
        {
            static const struct 
            {
                const char* attribute_name;
                int location;
            } attribute_info[] =
            {
                {GLProgram::ATTRIB_NAME_POSITION,GLProgram::ATTRIB_POSITION},
                {GLProgram::ATTRIB_NAME_COLOR,GLProgram::ATTRIB_COLOR},
                {GLProgram::ATTRIB_NAME_NORMAL,GLProgram::ATTRIB_NORMAL},
                {GLProgram::ATTRIB_NAME_TANGENT,GLProgram::ATTRIB_TANGENT},
                {GLProgram::ATTRIB_NAME_TEX_COORD,GLProgram::ATTRIB_TEX_COORD},
                {GLProgram::ATTRIB_NAME_TEX_COORD1,GLProgram::ATTRIB_TEX_COORD1},
                {GLProgram::ATTRIB_NAME_TEX_COORD2,GLProgram::ATTRIB_TEX_COORD2},
                {GLProgram::ATTRIB_NAME_TEX_COORD3,GLProgram::ATTRIB_TEX_COORD3},
            };

            for(int i=0;i<GLProgram::ATTRIB_MAX;i++)
            {
                glBindAttribLocation(gl_program.program,attribute_info[i].location,attribute_info[i].attribute_name);
            }
        }
        
        void parse_vertex_attribs(GLProgram& gl_program)
        {
            GLint active_attribs;
            GLint length;
            glGetProgramiv(gl_program.program,GL_ACTIVE_ATTRIBUTES,&active_attribs);
            
            if(active_attribs > 0)
            {
                VertexAttrib attrib;
                glGetProgramiv(gl_program.program,GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,&length);
                if(length > 0)
                {
                    GLchar* attrib_name = (GLchar*) alloca(length+1);
                    for(int i=0;i<active_attribs;i++)
                    {
                        glGetActiveAttrib(gl_program.program,i,length,nullptr,&attrib.size,&attrib.type,attrib_name);
                        attrib_name[length] = '\0';
                        attrib.name = std::string(attrib_name);
                        attrib.index = glGetAttribLocation(gl_program.program,attrib_name);
                        gl_program.attribs[attrib.name] = attrib;
                    }
                }
            }
        }

        void parse_uniforms(GLProgram& gl_program)
        {
            GLint active_uniforms;
            GLint length;
            glGetProgramiv(gl_program.program,GL_ACTIVE_UNIFORMS,&active_uniforms);

            if(active_uniforms > 0)
            {
                Uniform uniform;
                glGetProgramiv(gl_program.program,GL_ACTIVE_UNIFORM_MAX_LENGTH,&length);
                if(length > 0)
                {
                    GLchar* uniform_name = (GLchar*) alloca(length+1);
                    for(int i=0;i<active_uniforms;i++)
                    {  
                        glGetActiveUniform(gl_program.program,i,length,nullptr,&uniform.size,&uniform.type,uniform_name);

                        uniform_name[length] = '\0';
                        /// If starts with u_
                        printf("Uniform name %s %d %d \n",uniform_name,uniform.size,uniform.type);
                        
                        if(uniform_name[0] != 'u' && uniform_name[0] != 'l')
                        {
                            uniform.name = std::string(uniform_name);
                            uniform.index = glGetUniformLocation(gl_program.program,uniform_name);
                            gl_program.custom_uniforms[uniform.name] = uniform;
                        }

                        if(uniform_name[0] == 'l')
                        {
                            uniform.name = std::string(uniform_name);
                            uniform.index = glGetUniformLocation(gl_program.program,uniform_name);
                            gl_program.light_uniforms[uniform.name] = uniform;
                        }

                    }
                    
                }
            }
        }

        std::string log_shader(uint32_t shader)
        {
            GLint log_length = 0;
            glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&log_length);
            if(log_length < 1)
                return "";
            
            char* log_bytes = (char*) malloc(sizeof(char)*log_length);
            glGetShaderInfoLog(shader,log_length,nullptr,log_bytes);
            std::string r(log_bytes);
            free(log_bytes);

            return r;
        }

        void clear_shader(GLProgram& gl_program)
        {
            if(gl_program.vert_shader)
            {
                glDeleteShader(gl_program.vert_shader);
            }

            if(gl_program.frag_shader)
            {
                glDeleteShader(gl_program.frag_shader);
            }

            gl_program.vert_shader = 0;
            gl_program.vert_shader = 0;
        }

        void destroy(GLProgram& gl_program)
        {
            clear_shader(gl_program);
            if(gl_program.program)
            {
                glDeleteProgram(gl_program.program);
            }

            gl_program.program = 0;
        }
    }
}
