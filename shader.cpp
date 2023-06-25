///////////////////////////////////////////////////////////////////////////////
// Interface for creating shader programs.
///////////////////////////////////////////////////////////////////////////////

#include "shader.h"

#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

GLchar* alloc_file_str(const char* file_name)
{
    assert(file_name != NULL);
    int fd = open(file_name, O_RDONLY);
    if (fd < 0)
    {
        perror(file_name);
        exit(EXIT_FAILURE);
    }
    struct stat file_info;
    if (fstat(fd, &file_info) < 0)
    {
        perror(file_name);
        close(fd);
        exit(EXIT_FAILURE);
    }
    // One extra for string terminator.
    GLchar* code_str = (GLchar*)malloc(file_info.st_size + 1);
    if (code_str == NULL)
    {
        perror("allocating file buffer");
        close(fd);
        exit(EXIT_FAILURE);
    }
    memset((void*)code_str, 0, file_info.st_size + 1);
    ssize_t bytes_read = read(fd, code_str, file_info.st_size);
    if (bytes_read < 0)
    {
        perror(file_name);
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    return code_str;
}

void print_error(GLuint target, const char* msg, const char* type)
{
    GLint size = 0;
    glGetShaderiv(target, GL_INFO_LOG_LENGTH, &size);
    if (size == 0)
    {
        fprintf(stderr, "%s -- %s\n", msg, type);
        fprintf(stderr, "No error log.\n");
        exit(EXIT_FAILURE);
    }
    GLchar* info_log = (GLchar*)malloc(size);
    if (info_log == NULL)
    {
        perror("allocating glsl compile log");
        exit(EXIT_FAILURE);
    }
    glGetShaderInfoLog(target, size, NULL, info_log);
    fprintf(stderr, "%s -- %s\n", msg, type);
    fprintf(stderr, "%s\n", info_log);
    free(info_log);
    exit(EXIT_FAILURE);
}

void check_link_error(GLuint program)
{
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (not success)
    {
        print_error(program, "GLSL PROGRAM LINK ERROR", "LINK");
    }
}

void check_compile_error(GLuint shader, const char* type)
{
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (not success)
    {
        print_error(shader, "GLSL SHADER COMPILE ERROR", type);
    }
}

void compile_shader(GLuint shader, const char* src_file, const char* type)
{
    const GLchar* code = alloc_file_str(src_file);
    glShaderSource(shader, 1, &code, NULL);
    free((void*)code);
    glCompileShader(shader);
    check_compile_error(shader, type);
}

GLuint create_shader_program(const char* vert_shader_file, const char* geom_shader_file, const char* frag_shader_file)
{
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(vertex, vert_shader_file, "VERTEX");
    GLuint geometry = glCreateShader(GL_GEOMETRY_SHADER);
    if (geom_shader_file != NULL)
    {
        compile_shader(geometry, geom_shader_file, "GEOMETRY");
    }
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(fragment, frag_shader_file, "FRAGMENT");
    GLuint prog_id = glCreateProgram();
    glAttachShader(prog_id, vertex);
    if (geom_shader_file != NULL)
    {
        glAttachShader(prog_id, geometry);
    }
    glAttachShader(prog_id, fragment);
    glLinkProgram(prog_id);
    check_link_error(prog_id);
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
    return prog_id;
}
