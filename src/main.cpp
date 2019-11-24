#include "JackAnalyzer.hpp"
#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"

/*
//split a string 
vector<string> split(const string& str, char c)
{
	vector<string> result;
	string temp = "";
	for (int i = 0;i < str.size();i++) {
		if (str[i] != c) {
			temp.push_back(str[i]);
		}
		else {
			result.push_back(temp);
			temp = "";
		}
	}
	if (temp.size())
		result.push_back(temp);
	return result;
}


void getFileName(string inputName, vector<string>& inputNames, vector<string>& outputNames) {

	if (inputName.size() > 5 && inputName.substr(inputName.size() - 5) == ".jack") {
		// single .jack file
		cout << "input " << inputName << endl;
		inputNames.push_back(inputName);
		outputNames.push_back(inputName.substr(0, inputName.size() - 2) + ".T.xml");
	}
	else {
		// a directory containing many .jack files

		std::string path = inputName;
		//outputName = inputName + "\\" + *(split(path, '\\').end() - 1) + ".T.xml";
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string fileName = entry.path().string();
			cout << "input "<< fileName << endl;
			if (fileName.size() > 5 && fileName.substr(fileName.size() - 5) == ".jack") {
				// single .jack file

				inputNames.push_back(fileName);
				outputNames.push_back(fileName.substr(0, fileName.size() - 5) + ".Txml1");
			}
		}
	}
}
*/
void tokenize(string inputName, string outputName) {
	JackTokenizer jackTokenizer(inputName);
	ofstream outputFile;
	outputFile.open(outputName);
	outputFile << "<tokens> " << endl;
	while (jackTokenizer.hasMoreTokens()) {
		jackTokenizer.advance();
		if (jackTokenizer.tokenType() == KEYWORD) {
			outputFile << "<keyword> " << Keywords[jackTokenizer.keyWord()] << " </keyword>" << endl;

		}
		else if (jackTokenizer.tokenType() == STRING_CONSTANT) {
			outputFile << "<stringConstant> " << jackTokenizer.stringVal() << " </stringConstant>" << endl;
		}
		else if (jackTokenizer.tokenType() == INT_CONSTANT) {
			outputFile << "<integerConstant> " << jackTokenizer.intVal() << " </integerConstant>" << endl;
		}
		else if (jackTokenizer.tokenType() == IDENTIFIER) {
			outputFile << "<identifier> " << jackTokenizer.identifier() << " </identifier>" << endl;
		}
		else if (jackTokenizer.tokenType() == KEYWORD) {
			outputFile << "<keyword> " << jackTokenizer.keyWord() << " </keyword>" << endl;
		}
		else if (jackTokenizer.tokenType() == SYMBOL) {
			string output = jackTokenizer.symbol();
			if (jackTokenizer.symbol() == "<")
				output = "&lt;";
			else if (jackTokenizer.symbol() == ">")
				output = "&gt;";
			else if (jackTokenizer.symbol() == "\"")
				output = "&quot;";
			else if (jackTokenizer.symbol() == "&")
				output = "&amp;";

			outputFile << "<symbol> " << output << " </symbol>" << endl;
		}
	}
	outputFile << "</tokens> " << endl;
	outputFile.close();
}
int main(int argc, char* argv[])
{
	string sourceName;
	if (argc > 1)
		sourceName = argv[1];
	else
	{
		cout << "Command line parameter should be name of input file or fold" << endl;
		return -1;
	}
	
	shared_ptr<JackAnalyzer> jackAnalyzer(new JackAnalyzer());
	jackAnalyzer->analyze(sourceName);
	
	return 0;
}