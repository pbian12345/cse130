#!/bin/bash

#######################################################################
#
# Copyright (C) 2020-2021 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

echo ""
echo "CSE130 Winter 2021 Assignment 2"
echo ""
date

./check.sh | tee check.out

ptotal=0
pcnt=0
for pct in `cat check.out | grep 'Tests' | grep '\%' | sed 's/\%//' | awk -F "/" '{print $2}' | awk -F " " '{print $2}'`
do
    ptotal=`echo "scale=2; $ptotal + $pct" | bc -l`
    (( pcnt += 1 ))
done

total=0;
if (( pcnt > 0 ))
then
    total=`echo "scale=2; ($ptotal / $pcnt) * 0.9" | bc -l`
fi
rm check.out

ccode=0
if [ ! -s make.out ]
then
    (( ccode = 100 ))
fi
printf "%20s:         %5.1f%% of 10%%\n" "C-Code" $ccode 

total=`echo "scale=2; $total + ($ccode*0.1)" | bc -l`
printf "\n%20s: %5.1f%%\n\n" "Total" $total 

