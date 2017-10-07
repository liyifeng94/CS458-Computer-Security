CXX = g++
CXXFLAGS = -std=c++11 -Wall
OBJECTS = main.o
DEPENDS = ${OBJECTS:.o=.d}
BIN = ids
SUMBIT_FILES = main.cpp issExp.h dns.h ntp.h Makefile domains.txt

all : ${BIN}

${BIN}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${BIN} -lpcap

debug: ${OBJECTS}
	${CXX} ${CXXFLAGS} -ggdb ${OBJECTS} -o ${BIN} -lpcap

-include ${DEPENDS}

pack:
	tar -cvf src.tar ${SUMBIT_FILES}

clean:
	rm -f ${DEPENDS} ${OBJECTS} ${BIN}
	rm -f ./src.tar
