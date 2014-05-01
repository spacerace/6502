#!/bin/bash

for i in `ls *.bz2`
do
	echo "echo unzipping $i"
	bunzip2 $i
done
