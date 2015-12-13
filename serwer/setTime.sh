#!/bin/bash

if [ $# -ne 2 ] 
then
    echo "Usage: ./setTime.sh HH:MM AM/PM"
else
    echo "Time to be set: $1"
    echo "AM/PM: $2"
    date +"%T %p" -s "$1 $2"
fi


