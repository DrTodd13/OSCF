all:	supp.exe allstars.exe

supp.exe:	supplements.cc
	g++ -std=c++11 -o supp.exe supplements.cc

allstars.exe:	allstars.cc DatFile.h
	g++ -std=c++11 -o allstars.exe allstars.cc

clean:
	rm supp.exe allstars.exe
