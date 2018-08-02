#version 330

in vec3 a_Position;
in vec2 a_uvPosition;

out vec2 v_TexPos;
out vec2 v_LinePos;

void main()
{
	gl_Position = vec4(a_Position, 1);

	v_TexPos = a_uvPosition;

	v_LinePos.x = (a_Position.x * 2) * 3.141592f;
	v_LinePos.y = (a_Position.y * 2);

}
