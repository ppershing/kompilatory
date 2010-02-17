.PHONY: default

default: compiler metacompiler

.PHONY: all

all: compiler metacompiler examples

.PHONY: compiler
compiler: 
	(cd compiler; make)

.PHONY: metacompiler
metacompiler: 
	(cd metacompiler; make)

.PHONY: examples
examples: 
	(cd examples; make)

.PHONY: clean
clean:
	-(cd metacompiler; make clean)
	-(cd compiler; make clean)
	-(cd examples; make clean)

.PHONY: package
package:
	tar -cvvf kompilatory.tar *
