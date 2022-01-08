#!/bin/bash

../get_ratings_data.sh

cp ../homeschool.txt .

# Command-line is get_all_stars.sh <nwsrs tournament results file.txt> <start date MM-DD-YYYY> <end date MM-DD-YYYY>
echo "Extracting cumulative tournament report data"
../extract_cumul_tourney_reports.sh $1 > cumul.out

#for f in *.dbf
#do
#	echo "Processing $f"
#	../get_or_supplement.awk $f > $f.or
#done
echo "Processing USCF supplementals"
for f in RSQ*.zip
do
    base="${f%.*}"
    unzip $f
    cp $base/TARSFLE1.DBF $base.DBF
    rm -rf $base
done

for f in *.DBF
do
	echo "Processing $f"
	../get_or_supplement.awk $f > $f.or
done

echo "Extracting USCF"
../supp.exe *.or > supp.out

echo "Calculating allstars"
echo ../allstars.exe rate* cumul.out supp.out $2 $3
../allstars.exe rate* cumul.out supp.out $2 $3 $4
