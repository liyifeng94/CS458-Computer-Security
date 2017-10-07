#!/bin/bash

src/ids test/q5-ping.pcap > test/q5a-output.log

diffResults=`diff test/q5-ping-output.log test/q5a-output.log`


if [ ! -z "$diffResults" ]; then
    echo "-----q5a Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q5-ping-output.log
    echo "-----Actual:-----"
    cat test/q5a-output.log	
    echo ""
else
    echo "-----q5a Test Passed-----"
fi

src/ids test/q5-syn.pcap > test/q5b-output.log

diffResults=`diff test/q5-syn-output.log test/q5b-output.log`

if [ ! -z "$diffResults" ]; then
    echo "-----q5b Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q5-syn-output.log
    echo "-----Actual:-----"
    cat test/q5b-output.log	
    echo ""
else
    echo "-----q5b Test Passed-----"
fi
