#!/bin/bash

../dbf2csv/dbf2csv.py $1 >& /dev/null
filename=$(basename "$1")
filename="${filename%.*}"
sed 's/\*//' $filename.csv | grep -v --line-buffered "INACTIVE ID" | gawk -F, '$6 ~ /^[0-9]+$/ {if ($5=="OR") print $3 " " $6 }' | gawk -F/ '{print $1}'
