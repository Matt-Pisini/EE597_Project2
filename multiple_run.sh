#!/bin/bash

RUN_SCRIPT_A_node='bash run.sh A node'
RUN_SCRIPT_A_rate='bash run.sh A rate'
RUN_SCRIPT_B_node='bash run.sh B node'
RUN_SCRIPT_B_rate='bash run.sh B rate'


for ((i=0; i<5; i++))
do
    eval $RUN_SCRIPT_A_node $i
done