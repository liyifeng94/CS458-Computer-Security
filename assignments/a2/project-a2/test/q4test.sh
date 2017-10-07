#!/bin/bash

src/ids test/q4-hosts.pcap > test/q4-output.log

diffResults=`diff test/q4-hosts-output.log test/q4-output.log`


if [ ! -z "$diffResults" ]; then
    echo "-----q4 Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q4-hosts-output.log
    echo "-----Actual:-----"
    cat test/q4-output.log	
    echo ""
else
    echo "-----q4 Test Passed-----"
fi
