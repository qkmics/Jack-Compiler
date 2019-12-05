#ifndef _VMWRITER_
#define _VMWRITER_
#include "Common.hpp"

class VMWriter {
public:
	VMWriter(string outputFile);
	~VMWriter();

	// write vm commands into a file
	void writePush(Segment seg, int index);
	void writePop(Segment seg, int index);
	void writeArithmetic(Command command);
	void writeLabel(string label);
	void writeGoto(string label);
	void writeIf(string label);
	void writeCall(string name, int nArgs);
	void writeFunction(string name, int nLocals);
	void writeReturn();
	
private:
	ofstream outputStream;

};

#endif