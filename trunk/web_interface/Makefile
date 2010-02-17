BIN=../../bin
META=../../../metacompiler/metacompiler

.PHONY: default
default: compiler automat_graph.png

compiler: ${BIN}/main.o ${BIN}/binsearch.o ${BIN}/stdio.o ${BIN}/automat.o \
	../../semantika.cpp table.o
	echo "Compiling compiler"
	g++ -o $@ $^

automat_graph.png: compiler
	echo "Generating graph of compilar automat";
	dot -Tpng < syntax.txt.dot > automat_graph.png

table.o: table.s
	echo "Compiling table"
	as -o $@ $^

table.s: ${META} syntax.txt
	echo "Running metacompiler to produce table";
	${META} syntax.txt > table.s

.PHONY: run
run: graph.png output

output: compiler input
	echo "compiling input";
	./compiler < input > output

graph.png: output
	echo "Making graph";
	dot -Tpng < graph.dot > graph.png

