#ifndef __FILE_IO_H__
#define __FILE_IO_H__

#include <string>
#include <sstream>
#include <iostream> 
#include <fstream> 

namespace render_engine
{
    namespace file_io
    {
        std::string read_text(const char* path)
        {
            std::string data;
            std::ifstream file;

            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try
            {
               file.open(path);
               std::stringstream stream;
               stream << file.rdbuf();
               file.close();
               data = stream.str();
            }
            catch(std::ifstream::failure e)
            {
                std::cerr << e.what() << '\n';
            }
            return data;
        }
    }
}

#endif