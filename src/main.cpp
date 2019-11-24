#include "JackAnalyzer.hpp"
#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"

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