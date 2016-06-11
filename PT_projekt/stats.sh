#!/bin/bash

if [ $# -ne 2 ] ; then
	echo "Needs 2 args, Jeez!"
	exit 1
fi

ITER_COUNT=25

sum=0
for i in $(seq 1 $ITER_COUNT) ; do
	echo -n "($i/$ITER_COUNT)"
	result=$(./devel/PT_projekt.exe --no-show "$1" "$2" | grep rating | cut -d $'\t' -f 2)
	sum=$(awk "BEGIN { print($sum + $result) }")
	average=$(awk "BEGIN { print($sum / $i) }")
	echo -e "\t$average ($result)"
done
	
exit 0
