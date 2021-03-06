#!/bin/bash
# INPUTS TO SCRIPT
# 1) input "A" or "B" meaning case A (CW = [1,1023]) or case B (CW = [63,127])
# 2) input "node" or "rate" meaning vary nodes (E1) or vary rate (E2)
# 3) input file_num for which # file to save output to

SRC_FILE='sim2_v2.cc'
DEST_PATH='/home/ee597/ns-3-allinone/ns-3-dev/scratch/'
SRC_PATH=$PWD
DEST_RUN='/home/ee597/ns-3-allinone/ns-3-dev/'
OUTPUT_FILE_RATE="vary_rate$3.txt"
OUTPUT_FILE_NODE="vary_node$3.txt"
HELP_OUTPUT=$'Input 1 = "A" or "B" meaning case A (CW = [1,1023]) or case B (CW = [63,127])
Input 2 = "node" or "rate" meaning vary nodes (E1) or vary rate (E2)'

# Command parameters
FIXED_NUM_NODES='20'
NUM_NODES='--N='
FIXED_DATA_RATE='10'
DATA_RATE='--Data_Rate='
COMMAND='./waf --run'
CASE='--CASE='
PIPE_OUTPUT='tail -n 1' #last line of stdout

cp $SRC_FILE $DEST_PATH
cd $DEST_RUN

if [[ $1 != "A" && $1 != "B" ]]; then
    echo "Incorrect input for 1st parameter."
    echo "$HELP_OUTPUT"
    exit
fi

if [[ $2 == "node" ]]; then
    for ((i=2; i<=4; i++))
    do
        eval "$COMMAND \"$SRC_FILE $CASE$1 $DATA_RATE$FIXED_DATA_RATE $NUM_NODES$i\"" | $PIPE_OUTPUT >> $OUTPUT_FILE_NODE
    done
    OUTPUT_FILE=$OUTPUT_FILE_NODE
elif [[ $2 == "rate" ]]; then
    for ((i=1; i<=20; i++))
    do
        eval "$COMMAND \"$SRC_FILE $CASE$1 $DATA_RATE$i $NUM_NODES$FIXED_NUM_NODES\"" | $PIPE_OUTPUT >> $OUTPUT_FILE_RATE
    done
    OUTPUT_FILE=$OUTPUT_FILE_RATE
else
    echo "Incorrect input for 2nd parameter."
    echo "$HELP_OUTPUT"
    exit
fi

cp $OUTPUT_FILE $SRC_PATH
rm $OUTPUT_FILE
echo "Finished running tasks"

# Run Python script & SCP results
# PY_SCRIPT='analyze.py'
# SCP_COMMAND='*.txt Ms.Al@192.168.86.22:/Users/Ms.Al/Desktop/Matt'
# cd $SRC_PATH
# eval "python $PY_SCRIPT $OUTPUT_FILE $2"
# scp $SCP_COMMAND
