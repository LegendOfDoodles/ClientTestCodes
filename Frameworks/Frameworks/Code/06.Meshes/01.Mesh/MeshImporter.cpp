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

CAnimationImporter::CAnimationImporter()
{
}

void CAnimationImporter::LoadAnimationData(char * in)
{
	std::ifstream ifs(in);
	std::string line;

	while (getline(ifs, line) && line.find("<skeleton count>", 0) == -1)
	{
	}
	getline(ifs, line);
	int skcount;
	char *end = NULL;
	skcount = strtof(line.c_str(), &end);
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
		skeleton.m_m4x4Matrix._11 = strtof(line.c_str(), &Matend);
		skeleton.m_m4x4Matrix._12 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._13 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._14 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._21 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._22 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._23 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._24 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._31 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._32 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._33 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._34 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._41 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._42 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._43 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._44 = strtof(Matend, NULL);

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

			aniMatrix.m_m4x4Matrix._11 = strtof(line.c_str(), &end);
			aniMatrix.m_m4x4Matrix._12 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._13 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._14 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._21 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._22 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._23 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._24 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._31 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._32 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._33 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._34 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._41 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._42 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._43 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._44 = strtof(end, NULL);
			it->m_Keyframe.push_back(aniMatrix);

		}

		++it;
	}
}

CAnimationImporter::~CAnimationImporter()
{
}
