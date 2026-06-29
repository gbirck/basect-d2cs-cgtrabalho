#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D Texture0;
uniform vec4 objColor;
uniform bool isTexured;

void main()
{
	if (isTexured)
	{
		vec4 texColor = texture(Texture0, TexCoords);
		FragColor = objColor * texColor;
	}
	else
		FragColor = objColor;
}

