# EE597_Project2
Authors: Matt Pisini & Will Gonzalez

FILE STRUCTURE:
- Sim2_v2.cc --> this is the main program containing NS-3 code where we calculate throughput and per-node throughput based on the input values of node number and data rate.
- We have two bash scripts:
    run.sh --> this script copies our Sim2_v2.cc file over to the correct NS-3 directory and then executes the program several times over a series of either changing node values or changing data rate values. These values are all written into a corresponding text file in the location of where you call run.sh
    multiple_run.sh --> this is a wrapper script that will call run.sh several times. producing many output files that we can then use to average our results for plotting.
- analyze.py --> this is a Python script that will read in our output files and save the graphs.

RUNNING SIMULATION:
- There are 3 main ways to run the program:
    1. Call multiple_run.sh and the result will be several output files for different trials.
        - e.g. 'bash multiple_run.sh'
        - NOTE: Values in output files are printed in the following format:
        -            *<Total_throughput> <throughput_per_node> <num_nodes> <data_rate>*
    2. Call run.sh and there will be a single output file for either varying nodes or varying data rate.
        - e.g. 'bash run.sh <CASE> <node or rate> <file_num>'
            - CASE is either "A" or "B" meaning case A (CW = [1,1023]) or case B (CW = [63,127])
            - 'node' will vary the nodes and 'rate' will vary the data rate
            - 'file_num' is an optional parameter. It is used when multiple_run.sh is called in order to differentiate output files.
    3. Directly call './waf --run "sim2_v2.cc --CASE=<A or B> --N=<num_nodes> --Data_Rate=<data_rate>" ' 
        - CASE: choose "A" or "B" meaning case A (CW = [1,1023]) or case B (CW = [63,127])
        - N: number of nodes to run the simulation with
        - Data_Rate: choose the data rate value in Mbps (e.g. '--Data_Rate=5' is 5Mbps)
        - Must be in '/home/ee597/ns-3-allinone/ns-3-dev/' directory to call execute.
        - Must copy sim2_v2.cc into '/home/ee597/ns-3-allinone/ns-3-dev/scratch/' directory.
