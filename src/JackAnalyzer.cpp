#include "JackAnalyzer.hpp"
#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"
#if defined _WIN32
#include <filesystem>
#elif defined __linux__
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
		// input sourceName is a directory containing many .jack files
		// need to extract all .jack file under this directory

		#if defined _WIN32
		// windows version
		
		for (const auto& entry : filesystem::directory_iterator(sourceName))
		{
			string fileName = entry.path().string();
			if (fileName.size() > 5 && fileName.substr(fileName.size() - 5) == ".jack") {
				// extract one .jack file

				cout << "Compiling " << fileName << endl;

				inputNames.push_back(fileName);
				outputNames.push_back(fileName.substr(0, fileName.size() - 5) + ".my.xml");
			}
		}

		#elif defined __linux__
		// linux version

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

				cout << "Compiling " << fileName << endl;

				inputNames.push_back(fileName);
				outputNames.push_back(fileName.substr(0, fileName.size() - 5) + ".my.xml");
			}
		}

		#endif
	}
}