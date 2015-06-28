#!/bin/bash

FILE=$1
PIXELS=$2

I=0

rm o.png
./scarve $FILE e.png c.png s.png o.png

while [ $I -lt $PIXELS ]
do
	rm tmp.png
	mv o.png tmp.png
	./scarve tmp.png e.png c.png s.png o.png > /dev/null
	
	I=$((I+1))
	echo $I
done
