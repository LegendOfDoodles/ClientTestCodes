#version 330

in vec3 a_Position;
in vec2 a_uvPosition;

uniform float u_Time;
uniform sampler2D uTexSampler;

out vec2 v_TexPos;

void main()
{
	gl_Position = vec4(a_Position, 1);
	
	int row = 0;	// x
	int col = 0;	// y

	vec2 newTexPos = a_uvPosition;

	ivec2 TextureSize = ivec2(0, 0);

	ivec2 temp = textureSize(uTexSampler, 0).xy;
	TextureSize.x = int(temp.x);
	TextureSize.y = int(temp.y);

	row = (TextureSize.x / 128);
	col = (TextureSize.y / 128);

	// u_Time == Animaiton Time (재생 시간) -> 끊기는 현상 발생?
	int newtime = int(floor(u_Time * 8));
	int u = newtime - int(row * floor(1.0f * newtime/row)); //mod(newtime, row);
	int v = (col - 1) - newtime / row;

	newTexPos.x = (a_uvPosition.x / row) + ((u / float(row)));
	newTexPos.y = (a_uvPosition.y / col) + ((v / float(col)));

	v_TexPos = newTexPos;
}
