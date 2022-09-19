all: compile link
	
compiler:="g++"
linker:="g++"
cppVersion:="c++17"
buildDir:="build"

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

linkMain:
	$(linker) $(buildDir)/*.o -lxml2 -o $(buildDir)/main
