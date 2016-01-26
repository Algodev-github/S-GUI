#!/bin/bash
#Check the dependecies of the benchmark suite

function check_dep {

	
	COMMAND_LIST=( "$@" )
	for i in "${COMMAND_LIST[@]}" 
	do
   	:
   	type $i >/dev/null 2>&1 || 
	            { echo >&2 "$i command it's not installed. Aborting..."; exit 1; }
	echo "Controlling command $i...yes"
	done
}

echo "Check principal dependencies..."
check_dep awk iostat bc time fio

echo "Check secondary dependencies..."
echo "If there are not all the dependencies, there will be no problem bro"
check_dep pv git make


