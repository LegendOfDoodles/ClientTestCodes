#version 330

in vec3 a_Position;

uniform float u_Time;

void main()
{
	vec3 newPos = a_Position;

	float CoolTime = u_Time / 10.f;
	newPos.y = sin(a_Position.x * 3.141592 + u_Time) * 0.05f + CoolTime -1.f;

	gl_Position = vec4(newPos, 1);
}
