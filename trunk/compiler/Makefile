BIN=bin

.PHONY: all
all: ${BIN}/main.o ${BIN}/stdio.o ${BIN}/automat.o ${BIN}/binsearch.o

${BIN}/main.o:main.s
	as -W -g -o ${BIN}/main.o main.s --32

${BIN}/automat.o:automat.s
	as -W -g -o ${BIN}/automat.o automat.s --32

${BIN}/binsearch.o:binsearch.s
	as -W -g -o ${BIN}/binsearch.o binsearch.s --32

${BIN}/stdio.o:stdio.s
	as -W -g -o ${BIN}/stdio.o stdio.s --32

.PHONY: clean
clean:
	-rm ${BIN}/*.o
