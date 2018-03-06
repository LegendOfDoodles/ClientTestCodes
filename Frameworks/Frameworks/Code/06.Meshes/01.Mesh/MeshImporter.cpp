#include "stdafx.h"
#include "MeshImporter.h"


CMeshImporter::CMeshImporter(char* in)
{
	std::ifstream ifs(in);

	std::string line;
	//삼각형 개수 확인
	while (std::getline(ifs, line))
	{
		if (line.find("triangles count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			m_iTriCnt = atoi(m.str(0).c_str());
			m_xmTriIndex.reserve(m_iTriCnt);
			break;
		}
	}
	//인덱스 추출
	while (std::getline(ifs, line) && line.find("</triangles>", 0) == -1)
	{
		std::regex number("[0-9]+");
		std::sregex_iterator it(line.begin(), line.end(), number);
		std::sregex_iterator end;
		float index[10];
		int n = 0;
		while (it != end)
		{
			std::smatch m = *it;
			index[n] = atoi(m.str(0).c_str());
			++it;
			++n;
		}
		m_xmTriIndex.push_back(XMFLOAT3(index[0], index[1], index[2]));
	}
	while (std::getline(ifs, line))
	{
		if (line.find("vertices count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			m_iVerticesCnt = atoi(m.str(0).c_str());
			m_Vertices.reserve(m_iVerticesCnt);
			break;
		}
	}

	while (std::getline(ifs, line) && line.find("</vertices>", 0) == -1)
	{
		MyVertex vertex;
		while (std::getline(ifs, line) && line.find("</vtx>", 0) == -1)
		{
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			if (line.find("<pos>", 0) != -1) {
				float index[10];
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
			else if (line.find("<norm>", 0) != -1) {
				float norm[10];
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					norm[n] = atof(m.str(0).c_str());
					if (n >= 3)break;
					++it;
					++n;
				}
				vertex.normal = XMFLOAT3(norm[0], norm[1], norm[2]);
			}
			else if (line.find("<sw>", 0) != -1) {
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					vertex.skinweight[n] = atof(m.str(0).c_str());
					++it;
					++n;
				}
			}
			else if (line.find("<si>", 0) != -1) {
				int n = 0;
				while (it != end)
				{
					std::smatch m = *it;
					vertex.skinindex[n] = atoi(m.str(0).c_str());
					++it;
					++n;
				}
			}
			else if (line.find("<tex>", 0) != -1) {
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
		m_Vertices.push_back(vertex);
	}
}


CMeshImporter::~CMeshImporter()
{
}
