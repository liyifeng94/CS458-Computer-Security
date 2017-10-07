#!/bin/bash

src/ids test/q6-unicode.pcap > test/q6-output.log

diffResults=`diff test/q6-unicode-output.log test/q6-output.log`


if [ ! -z "$diffResults" ]; then
    echo "-----q6 Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q6-unicode-output.log
    echo "-----Actual:-----"
    cat test/q6-output.log	
    echo ""
else
    echo "-----q6 Test Passed-----"
fi
