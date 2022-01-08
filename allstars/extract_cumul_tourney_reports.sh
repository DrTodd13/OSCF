#!/bin/sh

awk '/pos last name first/{flag=1;next}/The total Number/{flag=0}flag' $1 | cut -c28- | sed 's/\// /' | awk '{ num_games = 0; for(i=5; i < NF; i++) { if (($i ~ /[WLD]/) && ($i != "WF") && ($i != "LF")) num_games=num_games+1;} print $1 " " $2 " " $3 " " $4 " " num_games}'
#awk '/pos last name first/{flag=1;next}/The total Number/{flag=0}flag' $1 | cut -c28- | sed 's/\// /' | cut -c1-24 | awk '{if ($4 >= 15) print $0}'
