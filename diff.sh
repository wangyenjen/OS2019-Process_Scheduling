#!/bin/bash

sudo ./scheduler < $1 2> output.txt

./calc output.txt < $1

rm output.txt
