///////////////////////////////////////////////////////////////////////////////
// Interface for creating shaders.
///////////////////////////////////////////////////////////////////////////////

#ifndef Shader_Included_h
#define Shader_Included_h

#include "glad/glad.h"

GLuint create_shader_program(const char* vert_shader_file,
                             const char* geom_shader_file,
                             const char* frag_shader_file);

#endif // Shader_Included_h
