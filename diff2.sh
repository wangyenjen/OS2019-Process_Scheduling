#!/bin/bash

# sudo ./scheduler < $1 > output2.txt
# ./calc output2.txt < $1
for t in RR SJF PSJF FIFO ; do
    for i in {1..5} ; do
        echo "----------------------"$t"_$i----------------------------"
        sudo ./scheduler < "test/"$t"_$i.txt" 2> "outputs/schd_"$t"_$i.txt"
        ./calc "outputs/schd_"$t"_$i.txt" < "test/"$t"_$i.txt" &> "outputs/calc_"$t"_$i.txt"
    done
done
