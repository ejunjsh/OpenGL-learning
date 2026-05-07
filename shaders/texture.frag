#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D container;
uniform sampler2D face;

void main()
{
    FragColor = mix(texture(container, TexCoord), texture(face, TexCoord), 0.2);
}
