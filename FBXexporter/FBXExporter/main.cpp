#include<iostream>
#include "FBXExporter.h"
#include <string>

using namespace std;

int main(int argc, char** argv)
{
	string s;
	char inputname[100] = ".\\\\testModels\\\\";
	char inputname_last[] = ".fbx";

	char outputpath[] = ".\\\\exportedModels\\\\";
	cout << "export 하려는 파일이름을 입력하시오::";
	getline(cin, s);
	char *cstr = &s[0u];

	strcat_s(inputname, cstr);
	strcat_s(inputname, inputname_last);


	FBXExporter* myExporter = new FBXExporter();
	myExporter->Initialize();
	myExporter->LoadScene(inputname, outputpath);
	myExporter->ExportFBX();
	getchar();
	return 0;
}

