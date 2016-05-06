all:	supp.exe allstars.exe

supp.exe:	supplements.cc
	g++ -g -std=c++11 -o supp.exe supplements.cc

allstars.exe:	allstars.cc DatFile.h
	g++ -g -std=c++11 -o allstars.exe allstars.cc

clean:
	rm supp.exe allstars.exe
