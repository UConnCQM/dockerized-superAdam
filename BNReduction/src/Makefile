install :
	cd boolstuff-0.1.13  &&  ./configure
	cd boolstuff-0.1.13  &&  make 
	cd BuildAndNot && make
	cd NetReductionBoost  && make
	cd ToPolynomial && make
	cd MapFixedPoints && make

clean:
	-(cd boolstuff-0.1.13  &&  ./configure )
	-(cd boolstuff-0.1.13  &&  make distclean)
	-(cd BuildAndNot && make clean)
	-(cd NetReductionBoost  && make clean)
	-(cd ToPolynomial && make clean)
	-(cd MapFixedPoints && make clean)

