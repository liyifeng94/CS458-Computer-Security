CXX = g++-4.9
CXXFLAGS = -std=c++11 -Wall
OBJECTS = src/kanon.o src/cvsUtil.o
BIN = kanon

default: all

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

all: ${BIN}


${BIN}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${BIN}

pack:
	rm -rf ./submit/kanon
	rm -f ./submit/kanon.tar
	${MAKE} clean
	${MAKE} all
	./kanon 5 Fake_Disease_Record.csv
	./verify.sh
	mkdir submit/kanon
	mkdir submit/kanon/src
	cp -rf ./src ./submit/kanon/src
	cp Makefile ./submit/kanon/Makefile
	cp ./anonymized.csv ./submit/kanon/anonymized.csv
	cp ./non-kanonymized.csv ./submit/kanon/non-kanonymized.csv
	cp ./submit/README ./submit/kanon/README
	cd ./submit/kanon && tar -cvf kanon.tar ./
	mv ./submit/kanon/kanon.tar ./submit/kanon.tar

pack-only:
	rm -rf ./submit/kanon
	rm -f ./submit/kanon.tar
	mkdir submit/kanon
	mkdir submit/kanon/src
	cp -rf ./src ./submit/kanon/src
	cp Makefile ./submit/kanon/Makefile
	cp ./anonymized.csv ./submit/kanon/anonymized.csv
	cp ./non-kanonymized.csv ./submit/kanon/non-kanonymized.csv
	cp ./submit/README ./submit/kanon/README
	cd ./submit/kanon && tar -cvf kanon.tar ./
	mv ./submit/kanon/kanon.tar ./submit/kanon.tar


clean:
	-rm -f ${OBJECTS} ${BIN}