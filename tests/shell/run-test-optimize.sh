#!/bin/bash

find testcases/ | grep dumps | grep "\.nft$" | while read line
do
	echo "$line ..."
	nft -o -c -f $line
done
