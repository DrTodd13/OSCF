all:	cc_to_swiss.exe

cc_to_swiss.exe:	cc_to_swiss.cc ../include/tokenize_csv.h
	g++ -g -std=c++11 -o cc_to_swiss.exe cc_to_swiss.cc

clean:
	rm cc_to_swiss.exe
