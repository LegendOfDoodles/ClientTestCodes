#version 330

uniform sampler2D uTexSampler;

in vec2 v_TexPos;

layout(location=0) out vec4 FragColor;

void main()
{
	FragColor = texture(uTexSampler, v_TexPos).bgra;
}
