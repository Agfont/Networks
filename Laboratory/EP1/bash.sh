#!/bin/bash

if [ ! $# -eq 1 ]
then
  	echo "Falta argument"
   	exit 1
fi

re='^[0-9]+$'
if ! [[ $1 =~ $re ]]
then
   	echo "error: Not a number" >&2
   	exit 2

else
	num_clients=$1
	let it=$num_clients/2
	echo $it
	for (( c=0; c<$it; c++ ))
        do
		echo $c
		$(mosquitto_sub -t teste)
        	$(mosquitto_pub -t teste -m Palmeiras)
        done
        echo "Mensgens enviadas!"
	exit 0
fi

