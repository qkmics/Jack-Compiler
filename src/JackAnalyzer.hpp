#ifndef _JACK_ANALYSER_
#define _JACK_ANALYSER_
#include "Common.hpp"

class JackAnalyzer {
public:
	void analyze(string sourceName);
private:
	//vector<string> split(const string& str, char c);
	void getFileName(string sourceName, vector<string>& inputNames, vector<string>& outputNames);


};

#endif