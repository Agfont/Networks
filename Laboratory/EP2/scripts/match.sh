#!/bin/bash

# ./match.sh <addr> <port> <user1> <pwd1> <user2> <pwd2>
echo "Partida entre $3 e $5..."
./scripts/player2 $1 $2 $5 $6 > /dev/null &
./scripts/player1 $1 $2 $3 $4 $5 $6 > /dev/null &
wait
