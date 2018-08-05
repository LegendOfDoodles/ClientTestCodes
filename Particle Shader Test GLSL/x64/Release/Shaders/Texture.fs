#version 330

uniform float u_Time;
uniform sampler2D uTexSampler;
uniform sampler2D uTexSampler1;

in vec2 v_TexPos;
in vec2 v_LinePos;

layout(location=0) out vec4 FragColor;

void main()
{
	float CoolTime = u_Time / 10.f;
	vec4 t1 = texture(uTexSampler, v_TexPos).bgra;

	float newPosY;
	newPosY = sin(v_LinePos.x + u_Time) * 0.05f - 1.f + CoolTime;

	if (v_LinePos.y < newPosY){
		FragColor = t1;
	}

}
