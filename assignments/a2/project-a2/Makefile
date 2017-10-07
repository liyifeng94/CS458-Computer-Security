SRC_DIR = src
SRC_BIN = src/ids
TST_DIR = test
TST_SHS = q1test.sh q2test.sh q3test.sh q4test.sh q5test.sh q6test.sh q7test.sh
BIN = ids
TEST = runTests

default : ${BIN}

${BIN}:
	cd ${SRC_DIR} && ${MAKE}

debug:
	cd ${SRC_DIR} && ${MAKE} debug

install: ${BIN}
	rm -f ./${BIN}
	ln -s ${SRC_BIN}

test: install
	cp ${TST_DIR}/q1test.sh ./q1test.sh
	chmod u+x ./q1test.sh
	cp ${TST_DIR}/q2test.sh ./q2test.sh
	chmod u+x ./q2test.sh
	cp ${TST_DIR}/q3test.sh ./q3test.sh
	chmod u+x ./q3test.sh
	cp ${TST_DIR}/q4test.sh ./q4test.sh
	chmod u+x ./q4test.sh
	cp ${TST_DIR}/q5test.sh ./q5test.sh
	chmod u+x ./q5test.sh
	cp ${TST_DIR}/q6test.sh ./q6test.sh
	chmod u+x ./q6test.sh
	cp ${TST_DIR}/q7test.sh ./q7test.sh
	chmod u+x ./q7test.sh
	cp ${TST_DIR}/${TEST} ./${TEST}
	chmod u+x ./${TEST}
	cp src/domains.txt ./domains.txt

pack :
	cd ${SRC_DIR} && ${MAKE} pack
	cp src/src.tar submit/src.tar

clean:
	cd ${SRC_DIR} && ${MAKE} clean
	rm -f ${TEST} ${BIN}
	rm -f q*test.sh

cleanAll: clean
	rm -f *.log

testAndPack : test
	./${TEST}
	cd ${SRC_DIR} && ${MAKE} pack
	cp src/src.tar submit/src.tar