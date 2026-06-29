#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// texture sampler for the skybox
uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}

