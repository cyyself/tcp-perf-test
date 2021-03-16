#!/bin/bash
g++ client.cpp -o client
for ((i=1;i<=11000;i++))
do
./client &
done
