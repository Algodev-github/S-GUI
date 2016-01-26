#!/bin/bash
# Script used for retriving stats for each scheduler
#TO DO: USAGE
sched_0=${1-bfq}
sched_1=${2-cfq}
sched_2=${3-noop}
sched_3=${4-deadline}
NUM_READERS=${5-0}
NUM_WRITERS=${6-0}
RW_TYPE=${7-seq}
SECONDS=${8-0}
TEST_TYPE=${9-""}
TEST_DIR=${10-.}


# ERROR VARIABLES
ERROR_NO_SCHED=0
ERROR_FEW_DATA=-1
NO_ERROR=-2

# Go the correct test directory
CURRENT_DIR=$PWD
cd $TEST_DIR

# Arrays used to store avg of each parameter
# Parameter n°      0: BFQ data
# Parameter n°      1: CFQ data
# Parameter n°      2: NOOP data
# Parameter n°      3: DEADLINE data
AVG_VALUES=()

# Schedulers requested array
SCHEDULERS=($sched_0 $sched_1 $sched_2 $sched_3)

# Tests filenames array
FILENAMES=()

#file to manage
#output stats after test file
FILEOUT_AFTER="stats.out"

#echo
#echo "Requested data for :" ${SCHEDULERS[@]}
#echo "Value -> 0 = no request"
#echo "Value -> 1 = sched requested"
#echo "Order: Bfq Cfq Noop Deadline"
#echo

# Setup schedulers array
for i in 0 1 2 3
do
	if [  "${SCHEDULERS[$i]}" == "$ERROR_NO_SCHED" ]
	then
		AVG_VALUES[$i]="$ERROR_NO_SCHED"
	else
		AVG_VALUES[$i]="$NO_ERROR"
	fi
done


# Check if the seconds are setted
# SECONDS>0 AGGR TEST
# SECONDS==0 STARTUP_TEST
# Setup filenames array
if [ $SECONDS -ne 0 ]
then
	#echo "Aggr test detected..."
	for i in 0 1 2 3 
	do
		if [ "${AVG_VALUES[$i]}" == "$NO_ERROR" ]
		then
			FILENAMES[$i]=${SCHEDULERS[$i]}-${NUM_READERS}r${NUM_WRITERS}w-${RW_TYPE}-${SECONDS}sec-${TEST_TYPE}.txt
		fi
	done
else
	#echo "startup test detected..."
	for i in 0 1 2 3 
	do
		if [ "${AVG_VALUES[$i]}" == "$NO_ERROR" ]
		then
			FILENAMES[$i]=${SCHEDULERS[$i]}-${NUM_READERS}r${NUM_WRITERS}w-${RW_TYPE}-${TEST_TYPE}.txt
		fi
	done
fi

#echo "FILENAMES: " ${FILENAMES[@]}

# Handle few data error for each file
for i in 0 1 2 3 
do
	if [ "${FILENAMES[$i]}" != "" ]
	then
		ERROR=$(less ${FILENAMES[$i]} | awk 'FNR == '5'{print $1}')
		if [ "$ERROR" == "ERROR" -o "$ERROR" == "" ]
		then
			#echo "ERROR: too few data for file:" ${FILENAMES[$i]}
			#echo "ERROR VALUE: $ERROR_FEW_DATA"
			AVG_VALUES[$i]="$ERROR_FEW_DATA"
		fi
	fi
done

# Setup avg array
for i in 0 1 2 3
do
	if [ "${AVG_VALUES[$i]}" == "$NO_ERROR" ]
	then
		VALUE_AVG=$(less ${FILENAMES[$i]} | awk 'FNR == '4'{print $3}')
		AVG_VALUES[$i]="$VALUE_AVG"	
	fi	
done

#echo "creating after test file..."

##Create the file for data after test
if [ -f $FILEOUT_AFTER ]
then 
	rm -rf $FILEOUT_AFTER
fi

echo ${AVG_VALUES[@]} >> $FILEOUT_AFTER
#echo "FINAL AVG VALUES: "${AVG_VALUES[@]}

cd $CURRENT_DIR







