#include "VMWriter.hpp"

VMWriter::VMWriter(string outputFile){
	outputStream.open(outputFile);
}

VMWriter::~VMWriter() {
	outputStream.close();
}

void VMWriter::writePush(Segment seg, int index) {
	outputStream << "push " << SegmentString[(int)seg] << " " << index << endl;
}

void VMWriter::writePop(Segment seg, int index) {
	outputStream << "pop " << SegmentString[(int)seg] << " " << index << endl;
}

void VMWriter::writeArithmetic(Command command) {
	outputStream << CommandString[(int)command] << endl;
}

void VMWriter::writeLabel(string label) {
	outputStream << "label " << label << endl;
}

void VMWriter::writeGoto(string label) {
	outputStream << "goto " << label << endl;
}

void VMWriter::writeIf(string label) {
	outputStream << "if-goto " << label << endl;
}

void VMWriter::writeCall(string name, int nArgs) {
	outputStream << "call " << name << " " << nArgs << endl;
}

void VMWriter::writeFunction(string name, int nLocals) {
	outputStream << "function " << name << " " << nLocals << endl;
}

void VMWriter::writeReturn() {
	outputStream << "return" << endl;
}