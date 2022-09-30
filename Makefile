all: compile link
	
compiler:="g++"
linker:="g++"
cppVersion:="c++17"
buildDir:="build"

warn:
	@echo its recomended to create an alias to libxml2/libxml as libxml (by using "ln -s libxml2/libxml libxml" on linux like systems) to avoid errors from libxml2 header files.

compile: compileOperators compileError compileParser compileCairoInterpreter compileCairoInterpreter001 compileGisl compileMain

compileOperators:
	$(compiler) -std=$(cppVersion) -c src/operators.cpp -o $(buildDir)/operators.o

compileError:
	$(compiler) -std=$(cppVersion) -c src/error.cpp -o $(buildDir)/error.o

compileParser: 
	$(compiler) -std=$(cppVersion) -c src/parser.cpp -o $(buildDir)/parser.o

compileGisl: 
	$(compiler) -std=$(cppVersion) -c src/gisl.cpp -o $(buildDir)/gisl.o

compileMain: 
	$(compiler) -std=$(cppVersion) -c src/main.cpp -o $(buildDir)/main.o

compileCairoInterpreter:
	$(compiler) -std=$(cppVersion) -c src/cairoInterpreter.cpp -o $(buildDir)/cairoInterpreter.o

compileCairoInterpreter001:
	$(compiler) -std=$(cppVersion) -c src/cairoInterpreter0.0.1.cpp -o $(buildDir)/cairoInterpreter0.0.1.o

link:linkMain

onlyMain:
	g++ -std=c++17 ./src/main.cpp -lxml2 -lcairo build/*.o -o build/main

linkMain:
	$(linker) $(buildDir)/*.o -lxml2 -lcairo  -o $(buildDir)/graphml

