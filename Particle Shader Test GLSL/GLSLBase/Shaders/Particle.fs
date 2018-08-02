#version 330

uniform sampler2D uTexSampler;

in vec2 v_TexPos;

layout(location=0) out vec4 FragColor;

void main()
{
	
	FragColor = texture(uTexSampler, v_TexPos).bgra;
}

//for(int i = col-1; i == 0; --i){
	//	for (int j = 0; j < row; ++j){
	//		newTexPos.x = (v_TexPos.x / row) + ((1.f / row * floor(newtime)));
	//		newTexPos.y = (v_TexPos.y / col) + ((1.f / col) * i);
	//	}
	//}