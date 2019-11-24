#include "JackAnalyzer.hpp"
#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"
#include <filesystem>

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

		std::string path = sourceName;
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string fileName = entry.path().string();
			if (fileName.size() > 5 && fileName.substr(fileName.size() - 5) == ".jack") {
				// extract one .jack file

				cout << "input " << fileName << endl;

				inputNames.push_back(fileName);
				outputNames.push_back(fileName.substr(0, fileName.size() - 5) + ".my.xml");
			}
		}
	}
}
/*
#include <dirent.h>

JackAnalyzer::JackAnalyzer(string sourceName) {
	vector<string> inputFiles = getFileName(inputName, outputName);

	vector<vector<string>> fields;
	for (string inputFile : inputFiles)
	{
		
		vector<string> inLines = preprocessor.preprocess();				//preprocess input file
		Parser parser(extractFileName(inputFile));
		vector<vector<string>> fieldsOneFile = parser.parse(inLines);			//parse
		for (vector<string> field : fieldsOneFile)
			fields.push_back(field);
	}
	Code code(outputName);											//build code tables
	code.generateCode(fields);										//do translation

	cout << "Output file is " << outputName << endl;
}

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
		//directory
		struct dirent* ptr;
		DIR* dir;

		std::string path = inputName;
		dir = opendir(path.c_str());

		outputName = inputName + "/" + *(split(path, '/').end() - 1) + ".asm";

		//for (const auto& entry : experimental::filesystem::directory_iterator(path))
		while (ptr = readdir(dir))
		{
			if (ptr->d_name[0] == '.')
				continue;
			//string fileName = entry.path().string();
			string fileName = path + "/" + ptr->d_name;
			if (fileName.size() > 3 && fileName.substr(fileName.size() - 3) == ".vm")
				fileNames.push_back(fileName);
		}
	}
	return fileNames;
}

*/