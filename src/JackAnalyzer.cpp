#include "JackAnalyzer.hpp"
#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"
#if defined _WIN32
#include <filesystem>
#elif defined _LINUX_
#include <dirent.h>
#endif

void JackAnalyzer::analyze(string sourceName) {
	vector<string> inputNames, outputNames;
	getFileName(sourceName, inputNames, outputNames);
	for (int index = 0;index < inputNames.size();index++) {
		// analyze one file

		// create a JackTokenizer
		JackTokenizer jackTokenizer(inputNames[index]);

		// create a CompilationEngine
		CompilationEngine compilationEngine(outputNames[index]);

		// CompilationEngine do compiling
		compilationEngine.compileClass(jackTokenizer);
	}

}

void JackAnalyzer::getFileName(string sourceName, vector<string>& inputNames, vector<string>& outputNames) {

	if (sourceName.size() > 5 && sourceName.substr(sourceName.size() - 5) == ".jack") {
		// single .jack file
		cout << "input " << sourceName << endl;
		inputNames.push_back(sourceName);
		outputNames.push_back(sourceName.substr(0, sourceName.size() - 5) + ".my.xml");
	}
	else {
		// a directory containing many .jack files

		#if defined _WIN32

		for (const auto& entry : filesystem::directory_iterator(sourceName))
		{
			string fileName = entry.path().string();
			if (fileName.size() > 5 && fileName.substr(fileName.size() - 5) == ".jack") {
				// extract one .jack file

				cout << "input " << fileName << endl;

				inputNames.push_back(fileName);
				outputNames.push_back(fileName.substr(0, fileName.size() - 5) + ".my.xml");
			}
		}

		#elif defined _LINUX_

		struct dirent* ptr;
		DIR* dir;
		dir = opendir(sourceName.c_str());

		//for (const auto& entry : experimental::filesystem::directory_iterator(path))
		while (ptr = readdir(dir))
		{
			if (ptr->d_name[0] == '.')
				continue;
			//string fileName = entry.path().string();
			string fileName = sourceName + "/" + ptr->d_name;
			if (fileName.size() > 5 && fileName.substr(fileName.size() - 5) == ".jack") {
				// extract one .jack file

				cout << "input " << fileName << endl;

				inputNames.push_back(fileName);
				outputNames.push_back(fileName.substr(0, fileName.size() - 5) + ".my.xml");
			}
		}

		#endif
	}
}
/*
#include <dirent.h>

vector<string> JackAnalyzer::split(const string& str, char c)
{
	vector<string> result;
	string temp = "";
	for (int i = 0;i < str.size();i++)
	{
		if (str[i] != c)
		{
			temp.push_back(str[i]);
		}
		else
		{
			result.push_back(temp);
			temp = "";
		}
	}
	if (temp.size())
		result.push_back(temp);
	return result;
}

vector<string> JackAnalyzer::getFileName(const string& inputName, string& outputName)
{
	vector<string> fileNames;
	if (inputName.size() > 3 && inputName.substr(inputName.size() - 5) == ".jack") //singel file
	{
		fileNames.push_back(inputName);
		outputName = inputName.substr(0, inputName.size() - 5) + ".xml";
	}
	else
	{
		
	}
	return fileNames;
}

*/