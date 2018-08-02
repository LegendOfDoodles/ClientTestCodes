#version 330

uniform float u_Time;
uniform sampler2D uTexSampler;

in vec2 v_TexPos;

layout(location=0) out vec4 FragColor;

void main()
{

	vec2 newTexPos = v_TexPos;

	newTexPos.x = v_TexPos.x / 4;
	newTexPos.y = v_TexPos.y / 3;

	vec4 t1 = texture(uTexSampler, newTexPos).bgra;

	FragColor = t1;
}
