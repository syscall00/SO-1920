#!/bin/bash

SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
configFile="$SCRIPTPATH/config.txt"


if [ "${1,,}" == "easy" ]
    then
	echo "SO_BASE=60" > "$configFile"
	echo "SO_ALTEZZA=20" >> "$configFile"
	echo "SO_FLAG_MIN=5" >> "$configFile"
	echo "SO_FLAG_MAX=5" >> "$configFile"
	echo "SO_NUM_G=2" >> "$configFile"
	echo "SO_NUM_P=10" >> "$configFile"
	echo "SO_N_MOVES=20" >> "$configFile"
	echo "SO_ROUND_SCORE=10" >> "$configFile"
	echo "SO_MAX_TIME=3" >> "$configFile"
	echo "SO_MIN_HOLD_NSEC=100000000" >> "$configFile"

	echo "Parametri impostati su easy correttamente"

elif [ "${1,,}" == "hard" ]
    then
	echo "SO_BASE=120" > "$configFile"
	echo "SO_ALTEZZA=40" >> "$configFile"
	echo "SO_FLAG_MIN=5" >> "$configFile"
	echo "SO_FLAG_MAX=40" >> "$configFile"
	echo "SO_NUM_G=4" >> "$configFile"
	echo "SO_NUM_P=400" >> "$configFile"
	echo "SO_N_MOVES=200" >> "$configFile"
	echo "SO_ROUND_SCORE=200" >> "$configFile"
	echo "SO_MAX_TIME=1" >> "$configFile"
	echo "SO_MIN_HOLD_NSEC=100000000" >> "$configFile"

	echo "Parametri impostati su hard correttamente"
fi
