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

int main()
{

	// 입력파일을 input.txt라고 가정
	MyMesh mesh;

	ifstream ifs("FBXBinary//special01.meshinfo");
	string line;
	//삼각형 개수 확인
	while (getline(ifs, line))
	{
		if (line.find("triangles count", 0)!=-1) {
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
	while (getline(ifs, line) && line.find("</triangles>", 0)==-1)
	{
		std::regex number("[0-9]+");
		std::sregex_iterator it(line.begin(), line.end(), number);
		std::sregex_iterator end;
		float index[3];
		int n = 0;
		while (it != end)
		{
			std::smatch m = *it;
			index[n] = atoi(m.str(0).c_str());
			++it;
			++n;
		}
		mesh.m_xmTriIndex.push_back(XMFLOAT3(index[0], index[1], index[2]));
	}
	while (getline(ifs, line))
	{
		if (line.find("vertices count", 0)!=-1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			mesh.m_iVerticesCnt = atoi(m.str(0).c_str());
			mesh.m_xmVertex.reserve(mesh.m_iVerticesCnt);
			break;
		}
	}

	while (getline(ifs, line) && line.find("</vertices>", 0)==-1)
	{
		MyVertex vertex;
		while (getline(ifs, line) && line.find("</vtx>", 0)==-1)
		{
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			if (line.find("<pos>", 0)!=-1) {
				float index[3];
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					index[n] = atof(m.str(0).c_str());
					++it;
					++n;
				}
				vertex.pos = XMFLOAT3(index[0], index[1], index[2]);
			}
			else if (line.find("<norm>", 0)!=-1) {
				float norm[3];
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					norm[n] = atof(m.str(0).c_str());
					++it;
					++n;
				}
				vertex.normal = XMFLOAT3(norm[0], norm[1], norm[2]);
			}
			else if (line.find("<sw>", 0)!=-1) {
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					vertex.skinweight[n] = atof(m.str(0).c_str());
					++it;
					++n;
				}
			}
			else if (line.find("<si>", 0)!=-1) {
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					vertex.skinweight[n] = atoi(m.str(0).c_str());
					++it;
					++n;
				}
			}
			else if (line.find("<tex>", 0)!=-1) {
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					vertex.uv[n] = atoi(m.str(0).c_str());
					++it;
					++n;
				}
			}
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
	return 0;
}

