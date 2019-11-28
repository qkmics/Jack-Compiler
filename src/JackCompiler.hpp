#ifndef _JACK_ANALYSER_
#define _JACK_ANALYSER_
#include "Common.hpp"

class JackCompiler {
public:

	void analyze(string sourceName);

private:

	void getFileName(string sourceName, vector<string>& inputNames, vector<string>& outputNames);
};

#endif