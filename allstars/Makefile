all:	supp.exe allstars.exe allstars_for_web.exe

supp.exe:	supplements.cc
	g++ -g -std=c++11 -o supp.exe supplements.cc

allstars.exe:	allstars.cc DatFile.h
	g++ -g -std=c++11 -o allstars.exe allstars.cc

allstars_for_web.exe:	allstars_for_web.cc DatFile.h
	g++ -g -std=c++11 -o allstars_for_web.exe allstars_for_web.cc

clean:
	rm supp.exe allstars.exe allstars_for_web.exe
