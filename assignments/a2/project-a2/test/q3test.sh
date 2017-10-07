#!/bin/bash

src/ids test/q3-servers.pcap > test/q3-output.log

diffResults=`diff test/q3-servers-output.log test/q3-output.log`


if [ ! -z "$diffResults" ]; then
    echo "-----q3 Test Failed:-----"
    echo "-----Expected $expectOutputFile:-----"
    cat test/q3-servers-output.log
    echo "-----Actual:-----"
    cat test/q3-output.log	
    echo ""
else
    echo "-----q3 Test Passed-----"
fi
