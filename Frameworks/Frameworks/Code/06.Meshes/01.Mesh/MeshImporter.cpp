#include "stdafx.h"
#include "MeshImporter.h"


CMeshImporter::CMeshImporter()
{
	
}


void CMeshImporter::LoadMeshData(char * in)
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
		float index[3];
		int n = 0;
		char *end = NULL;
		index[0] = strtof(line.c_str(), &end);
		index[1] = strtof(end, &end);
		index[2] = strtof(end, NULL);
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
			m_xmVertex.reserve(m_iVerticesCnt);
			break;
		}
	}

	while (std::getline(ifs, line) && line.find("</vertices>", 0) == -1)
	{
		MyVertex vertex;
		int LineNum = 0;
		float index[3];

		while (std::getline(ifs, line) && line.find("</vtx>", 0) == -1)
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
		m_xmVertex.push_back(vertex);
	}
}

CMeshImporter::~CMeshImporter()
{
}
