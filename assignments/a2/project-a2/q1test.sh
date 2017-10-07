#!/bin/bash

src/ids test/q1-anomaly.pcap > test/q1-output.log

diffResults=`diff test/q1-anomaly-output.log test/q1-output.log`


if [ ! -z "$diffResults" ]; then
    echo "-----q1 Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q1-anomaly-output.log
    echo "-----Actual:-----"
    cat test/q1-output.log	
    echo ""
else
    echo "-----q1 Test Passed-----"
fi
