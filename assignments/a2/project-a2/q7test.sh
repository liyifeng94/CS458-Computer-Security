#!/bin/bash

src/ids test/q7-ntp.pcap > test/q7-output.log

diffResults=`diff test/q7-ntp-output.log test/q7-output.log`


if [ ! -z "$diffResults" ]; then
    echo "-----q7 Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q7-ntp-output.log
    echo "-----Actual:-----"
    cat test/q7-output.log	
    echo ""
else
    echo "-----q7 Test Passed-----"
fi
