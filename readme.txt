This program is a part of nand2teris project.

How to compile this code:
	Build a c++ project in a IDE and then add all files under src/ to the project can simply compile it. The compiler need to support c++ 17.
	In Linux Command Line, use "g++ -std=c++11 *.cpp -o output" in /src
	
How to run this code:
	After compiling, run the executable file. Name of input file should be the command line argument(inputfilename.vm or inputDirectory). 
Input can be in both absolute path and relative path. Can be both a file name or a directory.
	For every input .jack file, output a .my.xml file

What works and what doesn't work:
	This program has been tested on Unbuntu. All test files in nand2teris passed.
	This program has been tested on Windows. All test files in nand2teris passed. 