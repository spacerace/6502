#!/bin/sh

for i in `ls *.pdf`
do
	echo "bzipping $i"
	bzip2 -9 $i
done
