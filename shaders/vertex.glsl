#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vnormal;
out vec3 vcolor;
out vec3 frag_pos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    frag_pos = vec3(model * vec4(position, 1.0f));
    vnormal = normal;
    vcolor = color;
}

