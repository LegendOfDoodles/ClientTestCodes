// FBXexporter.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <fbxsdk.h>
#include <vector>
using namespace std;

struct MyVertex
{
	float pos[3];
	float normal[3];
	float uv[2];
};

FbxManager* g_pFbxSdkManager = nullptr;

HRESULT LoadFBX(std::vector<MyVertex>* pOutVertexVector)
{
	if (g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");

	bool bSuccess = pImporter->Initialize("../FBXexporter/Asset/SQ1.FBX", -1, g_pFbxSdkManager->GetIOSettings());
	if (!bSuccess) return E_FAIL;

	bSuccess = pImporter->Import(pFbxScene);
	if (!bSuccess) return E_FAIL;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

	if (pFbxRootNode)
	{
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			fbxsdk::FbxLayerElementUV *pFbxLayerElementUV = pMesh->GetLayer(0)->GetUVs();
			FbxVector4 normal;

			FbxVector4* pVertices = pMesh->GetControlPoints();

			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				
				for (int k = 0; k < iNumVertices; k++) {
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);


					MyVertex vertex;
					vertex.pos[0] = (float)pVertices[iControlPointIndex].mData[0];
					vertex.pos[1] = (float)pVertices[iControlPointIndex].mData[1];
					vertex.pos[2] = (float)pVertices[iControlPointIndex].mData[2];

					pMesh->GetPolygonVertexNormal(j, k, normal);
					
					vertex.normal[0] = (float)normal.mData[0];
					vertex.normal[1] = (float)normal.mData[1];
					vertex.normal[2] = (float)normal.mData[2];
										
					fbxsdk::FbxVector2 fbxUv = pFbxLayerElementUV->GetDirectArray().GetAt(iControlPointIndex);
					vertex.uv[0] = (float)fbxUv.mData[0];
					vertex.uv[1] = (float)fbxUv.mData[1];

					pOutVertexVector->push_back(vertex);
				}
			}

		}

	}
	return S_OK;
}

int main() {
	vector<MyVertex> output;
	//output->reserve(100);
	LoadFBX(&output);
	int count = 0;
	for (auto &d : output) {
		cout << "Position : ";
		cout <<  d.pos[0] << " " << d.pos[1] << " " << d.pos[2];
		
		cout << "  ,Normal : ";
		cout <<  d.normal[0] << " " << d.normal[1] << " " << d.normal[2] ;
		
		cout << "  ,UV : ";
		cout << d.uv[0] << " " << d.uv[1] << endl;
	}
	return 0;
}