// ImporterSample.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include <sstream>
#include <regex>
#include <Vector>

struct XMFLOAT3 {
	double x;
	double y;
	double z;
	XMFLOAT3() {}
	XMFLOAT3(float f1, float f2, float f3) :x(f1), y(f2), z(f3) {}
};

struct MyVertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	double skinweight[4];
	int skinindex[4];
	double uv[2];
};

struct MyMesh {
	int m_iTriCnt;
	vector<XMFLOAT3> m_xmTriIndex;
	int m_iVerticesCnt;
	vector<MyVertex> m_xmVertex;
};

struct XMMATRIX4X4 {
	float data[16];
};

struct KeyFrame {
	int m_iframe;
	XMMATRIX4X4 m_m4x4Matrix;
};

struct MySkeleton {
	int m_iID;
	int m_iParentID;
	
	XMMATRIX4X4 m_m4x4Matrix;
	vector<KeyFrame> m_Keyframe;
};

int main()
{
	vector<MySkeleton> skeletons;
	ifstream ifs("FBXBinary//test.aniinfo");
	string line;

	while (getline(ifs, line) && line.find("<skeleton count>", 0) == -1)
	{
	}
	getline(ifs, line);
	int skcount;
	char *end = NULL;
	skcount= strtof(line.c_str(), &end);
	skeletons.reserve(skcount);
	for (int i = 0; i < skcount; ++i) {
		MySkeleton skeleton;
		getline(ifs, line);
		float IDindex[2];
		char *end = NULL;
		skeleton.m_iID = strtof(line.c_str(), &end);
		skeleton.m_iParentID = strtof(end, NULL);

		char *Matend = NULL;
		getline(ifs, line);
		skeleton.m_m4x4Matrix.data[0]= strtof(line.c_str(), &Matend);


		for (int j = 1; j < 16; ++j) {
		skeleton.m_m4x4Matrix.data[j] = strtof(Matend, &Matend);
		}
		skeletons.push_back(skeleton);

	}



	while (getline(ifs, line) && line.find("<animations>", 0) == -1)
	{
	}
	
	auto it = skeletons.begin();
	for (int i = 0; i < skcount; ++i) {
		getline(ifs, line);
		char *end = NULL;
		KeyFrame aniMatrix;
		aniMatrix.m_iframe = strtof(line.c_str(), &end);

		while (getline(ifs, line) && line.find("</Joint>", 0) == -1)
		{
			end = NULL;
			int frameTime = strtof(line.c_str(), &end);
			getline(ifs, line);
			aniMatrix.m_m4x4Matrix.data[0] = strtof(line.c_str(), &end);
			for (int j = 1; j < 16; ++j) {
				aniMatrix.m_m4x4Matrix.data[j] = strtof(end, &end);
			}
			it->m_Keyframe.push_back(aniMatrix);

		}
		
		++it;
	}
	cout << "ani" << endl;
	/*
	// 입력파일을 input.txt라고 가정
	MyMesh mesh;

	ifstream ifs("FBXBinary//special01.meshinfo");
	string line;
	//삼각형 개수 확인
	while (getline(ifs, line))
	{
		if (line.find("triangles count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			mesh.m_iTriCnt = atoi(m.str(0).c_str());
			mesh.m_xmTriIndex.reserve(mesh.m_iTriCnt);
			break;
		}
	}
	//인덱스 추출
	while (getline(ifs, line) && line.find("</triangles>", 0) == -1)
	{
		float index[3];
		int n = 0;
		char *end = NULL;
		index[0] = strtof(line.c_str(), &end);
		index[1] = strtof(end, &end);
		index[2] = strtof(end, NULL);
		mesh.m_xmTriIndex.push_back(XMFLOAT3(index[0], index[1], index[2]));

	}
	while (getline(ifs, line))
	{
		if (line.find("vertices count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			mesh.m_iVerticesCnt = atoi(m.str(0).c_str());
			mesh.m_xmVertex.reserve(mesh.m_iVerticesCnt);
			break;
		}
	}

	while (getline(ifs, line) && line.find("</vertices>", 0) == -1)
	{
		MyVertex vertex;
		int LineNum = 0;
		float index[3];

		while (getline(ifs, line) && line.find("</vtx>", 0) == -1)
		{
			char *end = NULL;
			switch (LineNum)
			{
			case 0:

				index[0] = strtof(line.c_str(), &end);
				index[1] = strtof(end, &end);
				index[2] = strtof(end, NULL);
				vertex.pos = XMFLOAT3(index[0], index[1], index[2]);
				break;

			case 1:

				index[0] = strtof(line.c_str(), &end);
				index[1] = strtof(end, &end);
				index[2] = strtof(end, NULL);
				vertex.normal = XMFLOAT3(index[0], index[1], index[2]);
				break;;
			case 2:
				vertex.skinweight[0] = strtof(line.c_str(), &end);
				vertex.skinweight[1] = strtof(end, &end);
				vertex.skinweight[2] = strtof(end, &end);
				vertex.skinweight[3] = strtof(end, NULL);
				break;

			case 3:
				vertex.skinindex[0] = strtof(line.c_str(), &end);
				vertex.skinindex[1] = strtof(end, &end);
				vertex.skinindex[2] = strtof(end, &end);
				vertex.skinindex[3] = strtof(end, NULL);
				break;
			case 4:
				vertex.uv[0] = strtof(line.c_str(), &end);
				vertex.uv[1] = strtof(end, NULL);
				break;
			}
			LineNum++;
		}
		mesh.m_xmVertex.push_back(vertex);
	}

	cout << mesh.m_iTriCnt << endl;

	for (auto d : mesh.m_xmTriIndex) {
		cout << d.x << ", " << d.y << ", " << d.z << ", " << endl;
	}

	cout << mesh.m_iVerticesCnt << endl;
	for (auto d : mesh.m_xmVertex) {
		cout << "    pos: " << d.pos.x << ", " << d.pos.y << ", " << d.pos.z  <<endl;
		cout << " normal: " << d.normal.x << ", " << d.normal.y << ", " << d.normal.z  << endl;
		cout << " sindex:" << d.skinindex[0] << ", " << d.skinindex[1] << ", " << d.skinindex[2] <<endl;
		cout << "sweight:" << d.skinweight[0] << ", " << d.skinweight[1] << ", " << d.skinweight[2] << endl;
		cout << "     uv:" << d.uv[0] << ", " << d.uv[1] << endl;

	}
	// 화일 닫기
	ifs.close();
	*/



	return 0;
}

