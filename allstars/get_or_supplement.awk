#!/bin/bash

# Convert from DBF format to csv format.
../dbf2csv/dbf2csv.py $1 >& /dev/null
filename=$(basename "$1")
filename="${filename%.*}"
# Strip "*" from the file.
# Remove entries with INACTIVE ID in them.
# Find rows where the 6th field is numeric (contains some rating).
# Filter out rows where the player is not from OR.
# Filter out rows where the rating has a "/" in it indicating a provisional rating.
sed 's/\*//' $filename.csv | grep -v --line-buffered "INACTIVE ID" | gawk -F, '$6 ~ /^[0-9]+$/ {if ($5=="OR" && $6 !~ /[/]/) print $3 " " $6 }' | gawk -F/ '{print $1}'
