#ifndef _RHAPSODIES_SHADERREGISTRY
#define _RHAPSODIES_SHADERREGISTRY

#include <string>
#include <vector>
#include <map>

#include <GL/gl.h>

namespace rhapsodies {
  class ShaderRegistry {
    std::map<std::string, GLuint> _mapShader;
    std::map<std::string, GLuint> _mapProgram;
    std::map<std::string, std::map<std::string, GLuint> > _mapUniform;

  public:
    GLuint RegisterShader(std::string name,
                          GLenum type,
                          std::string path);

    GLuint RegisterProgram(std::string name,
                           std::vector<std::string> shader_names);

    GLuint RegisterUniform(std::string program_name,
                           std::string uniform_name);

    GLuint GetProgram(std::string name);
    GLuint GetUniform(std::string program_name,
                      std::string uniform_name);

  };
}

#endif // _RHAPSODIES_SHADERREGISTRY
