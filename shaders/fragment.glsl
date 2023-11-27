#version 460 core

in vec3 vnormal;
in vec3 vcolor;
in vec3 frag_pos;

// uniform vec3 color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vcolor, 1.0f);
}
