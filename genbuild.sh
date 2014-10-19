#!/bin/bash

awk -F',' '/movff/{$NF+=1;}1' OFS=','  buildnumber.txt >buildnumber.h
cp buildnumber.h buildnumber.txt
