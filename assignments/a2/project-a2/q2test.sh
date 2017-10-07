#!/bin/bash

src/ids test/q2-spoofed.pcap > test/q2-output.log

diffResults=`diff test/q2-spoofed-output.log test/q2-output.log`


if [ ! -z "$diffResults" ]; then
    echo "-----q2 Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q2-spoofed-output.log
    echo "-----Actual:-----"
    cat test/q2-output.log	
    echo ""
else
    echo "-----q2 Test Passed-----"
fi
