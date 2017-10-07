#!/bin/bash

expected0="8751f3e8fdbd7b620965a117245707ffdab848d0"
expected1="f7a1f4313c773f7066b50b75c1c20ba0268ad2b8"
expected2="9544191cc2adc5cf3c26e903ae24612e48ca0796"
expected3="916ee58bb6b7511099946b28d03c00bdff1ec129"
expected4="f42dc23122ca882a23ee31c8b05670bf8da12be0"
expected5="d6efe53eac01ca496a7cc1f3b5acbe38322700dd"

if [ -e Makefile ]; then make
fi

./kanon 5 Fake_Disease_Record.csv

output0=`sha1sum Fake_Disease_Record.csv | awk '{print $1}'`
output1=`sha1sum anonymized.csv.1 | awk '{print $1}'`
output2=`sha1sum anonymized.csv.2 | awk '{print $1}'`
output3=`sha1sum anonymized.csv.3 | awk '{print $1}'`
output4=`sha1sum anonymized.csv | awk '{print $1}'`
output5=`sha1sum non-kanonymized.csv | awk '{print $1}'`

if [ "$output0" == "$expected0" ]; then echo "Correct input file found"
else echo "Error: incorrect sha1sum for Fake_Disease_Record.csv !"
fi

if [ "$output1" == "$expected1" ]; then echo "Step 1 appears correct"
else echo "Error: incorrect sha1sum after Step 1!"
fi

if [ "$output2" == "$expected2" ]; then echo "Step 2 appears correct"
else echo "Error: incorrect sha1sum after Step 2!"
fi

if [ "$output3" == "$expected3" ]; then echo "Step 3 appears correct"
else echo "Error: incorrect sha1sum after Step 3!"
fi

if [ "$output4" == "$expected4" ]; then echo "anonymized.csv appears correct"
else echo "Error: incorrect sha1sum for anonymized.csv !"
fi

if [ "$output5" == "$expected5" ]; then echo "non-kanonymized.csv appears correct"
else echo "Error: incorrect sha1sum for non-kanonymized.csv !"
fi
