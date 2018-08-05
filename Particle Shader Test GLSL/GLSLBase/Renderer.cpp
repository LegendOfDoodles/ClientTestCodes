#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_CoolTimeShader = CompileShaders("./Shaders/DrawCoolTime.vs", "./Shaders/DrawCoolTime.fs");
	m_TextureShader = CompileShaders("./Shaders/Texture.vs", "./Shaders/Texture.fs");
	m_ParticleShader = CompileShaders("./Shaders/Particle.vs", "./Shaders/Particle.fs");

	//Create VBOs
	CreateVertexBufferObjects();

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-0.5, -0.5, 0.f, -0.5, 0.5, 0.f, 0.5, 0.5, 0.f, //Triangle1
		-0.5, -0.5, 0.f,  0.5, 0.5, 0.f, 0.5, -0.5, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	int pointCount = 100;
	float *Points = new float[(pointCount + 1) * 4];
	for (int i = 0; i <= pointCount; ++i)
	{
		Points[i * 4 + 0] = (i / (float)pointCount) * 2 - 1;
		Points[i * 4 + 1] = 0;
		Points[i * 4 + 2] = 0;
		Points[i * 4 + 3] = 1.f;
	}
	glGenBuffers(1, &m_VBOMovePoints);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOMovePoints);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (pointCount + 1) * 4, Points, GL_STATIC_DRAW);

	float TextureWLongRect[]
		=
	{
		-0.5f, -0.5f, 0.f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.f, 1.0f, 1.0f,
		 0.5f, -0.5f, 0.f, 1.0f, 0.0f
	};

	glGenBuffers(1, &m_VBOTexWLongRect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTexWLongRect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextureWLongRect), TextureWLongRect, GL_STATIC_DRAW);

	float ParticleTextureRect[]
		=
	{
		-0.5f, -0.5f, 0.f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.f, 0.0f, 1.0f,
		0.5f,  0.5f, 0.f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.f, 0.0f, 0.0f,
		0.5f,  0.5f, 0.f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.f, 1.0f, 0.0f
	};

	glGenBuffers(1, &m_VBOParticleRect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticleRect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleTextureRect), ParticleTextureRect, GL_STATIC_DRAW);

	char *imagePath = "./Texture/twice.bmp";
	unsigned int w, h;
	unsigned char *LoadTexture = loadBMPRaw(imagePath, w, h);

	glGenTextures(1, &m_gTexture);
	glBindTexture(GL_TEXTURE_2D, m_gTexture);
	// 실제로 GPU에 Allocation 하고 이미지를 올리는 함수
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, LoadTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	char *imagePath1 = "./Texture/Sprite2.bmp";
	unsigned int w1, h1;
	unsigned char *LoadTexture1 = loadBMPRaw(imagePath1, w1, h1);

	glGenTextures(1, &m_ParticleTexture);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);
	// 실제로 GPU에 Allocation 하고 이미지를 올리는 함수
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w1, h1, 0, GL_RGB, GL_UNSIGNED_BYTE, LoadTexture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::Test()
{
	glUseProgram(m_SolidRectShader);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);
}

void Renderer::Draw()
{
	GLuint shader = m_CoolTimeShader;

	glUseProgram(shader);

	GLint posId = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posId);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOMovePoints);
	glVertexAttribPointer(posId, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// uniform
	// DrawArray 전에 불려야 한다.
	GLuint id = glGetUniformLocation(shader, "u_Time");
	glUniform1f(id, m_fTimer);
	m_fTimer += 0.001f;
	
	glPointSize(3);
	glDrawArrays(GL_POINTS, 0, 100);
}

void Renderer::Texture()
{
	GLuint shader = m_TextureShader;
	glUseProgram(shader);

	GLint posID = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posID);

	GLint uvID = glGetAttribLocation(shader, "a_uvPosition");
	glEnableVertexAttribArray(uvID);

	GLuint id = glGetUniformLocation(shader, "u_Time");
	glUniform1f(id, m_fTimer);
	m_fTimer += 0.1f;

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOTexWLongRect);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLint uniformTex = glGetUniformLocation(shader, "uTexSampler");
	glUniform1i(uniformTex, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(posID);
	glDisableVertexAttribArray(uvID);
}

void Renderer::Particle()
{
	GLuint shader = m_ParticleShader;
	glUseProgram(shader);

	GLint posID = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(posID);

	GLint uvID = glGetAttribLocation(shader, "a_uvPosition");
	glEnableVertexAttribArray(uvID);

	GLuint id = glGetUniformLocation(shader, "u_Time");
	glUniform1f(id, m_fTimer);
	m_fTimer += 0.005f;

	glBindBuffer(GL_ARRAY_BUFFER, m_VBOParticleRect);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	GLint uniformTex = glGetUniformLocation(shader, "uTexSampler");
	glUniform1i(uniformTex, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ParticleTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(posID);
	glDisableVertexAttribArray(uvID);
	glDisableVertexAttribArray(uniformTex);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}