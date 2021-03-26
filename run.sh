#!/bin/bash
# INPUTS TO SCRIPT
# 1) input "A" or "B" meaning case A (CW = [1,1023]) or case B (CW = [63,127])
# 2) input "node" or "rate" meaning vary nodes (E1) or vary rate (E2)

SRC_FILE='sim2.cc'
DEST_PATH='/home/ee597/ns-3-allinone/ns-3-dev/scratch/'
SRC_PATH=$PWD
DEST_RUN='/home/ee597/ns-3-allinone/ns-3-dev/'
OUTPUT_FILE_RATE='vary_rate.txt'
OUTPUT_FILE_NODE='vary_node.txt'
HELP_OUTPUT=$'Input 1 = "A" or "B" meaning case A (CW = [1,1023]) or case B (CW = [63,127])
Input 2 = "node" or "rate" meaning vary nodes (E1) or vary rate (E2)'
PY_SCRIPT='analyze.py'
# Command parameters
FIXED_NUM_NODES='20'
NUM_NODES='--N='
FIXED_DATA_RATE='10'
DATA_RATE='--DATA_RATE='
COMMAND='./waf --run'
CASE='--CASE='


cp $SRC_FILE $DEST_PATH
cd $DEST_RUN
eval $COMMAND

if [[ $1 != "A" && $1 != "B" ]]; then
    echo "Incorrect input for 1st parameter."
    echo "$HELP_OUTPUT"
    exit
fi

if [[ $2 == "node" ]]; then
    for ((i=5; i<=50; i += 5))
    do
        eval "$COMMAND \"$SRC_FILE $CASE$1 $DATA_RATE$FIXED_DATA_RATE $NUM_NODES$i\"" >> $OUTPUT_FILE_NODE
    done
    OUTPUT_FILE=$OUTPUT_FILE_NODE
elif [[ $2 == "rate" ]]; then
    for ((i=5; i<=50; i += 5))
    do
        eval "$COMMAND \"$SRC_FILE $CASE$1 $DATA_RATE$i $NUM_NODES$FIXED_NUM_NODES\"" >> $OUTPUT_FILE_RATE
    done
    OUTPUT_FILE=$OUTPUT_FILE_RATE
else
    echo "Incorrect input for 2nd parameter."
    echo "$HELP_OUTPUT"
    exit
fi

cp $OUTPUT_FILE $SRC_PATH
cd $SRC_PATH
echo "python $PY_SCRIPT $OUTPUT_FILE $2"
# rm $OUTPUT_FILE