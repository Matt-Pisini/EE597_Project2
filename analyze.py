import os
import sys
import matplotlib.pyplot as plt

print(sys.argv)
input_file_name, input_type = sys.argv[1:]
input_file = open(input_file_name, 'r')
input_file_lines = input_file.readlines()
input_file.close()

T_list= []
N_list = []
T_per_node_list = []
DR_list = []

for item in input_file_lines:
    throughput, per_node_throughput, node_num, data_rate = item.split(' ')
    if input_type == "node":
        T_list.append(throughput)
        T_per_node_list.append(per_node_throughput)
        N_list.append(node_num)
        DR_list.append(data_rate)
    elif input_type == "rate":
        T_list.append(throughput)
        T_per_node_list.append(per_node_throughput)
        N_list.append(node_num)
        DR_list.append(data_rate)


if input_type == "node":
    plt.figure(1)
    plt.plot(N_list, T_per_node_list)
    plt.title("Saturation Throughput Per Node vs. Number of Nodes")
    plt.xlabel("Number of Nodes")
    plt.ylabel("Saturation Throughput Per Node")
    plt.savefig("throughput_per_node.png")
    # plt.show()    
    plt.figure(2)
    plt.plot(N_list, T_list)
    plt.title("Saturation Throughput vs. Number of Nodes")
    plt.xlabel("Number of Nodes")
    plt.ylabel("Saturation Throughput")
    plt.savefig("throughput_node.png")
    # plt.show()
elif input_type == "rate":
    plt.figure(1)
    plt.plot(DR_list, T_per_node_list)
    plt.title("Saturation Throughput Per Node vs. Data Rate")
    plt.xlabel("Date Rate")
    plt.ylabel("Saturation Throughput Per Node")
    plt.savefig("per_node_throughput_rate.png")
    # plt.show()
    plt.figure(2)
    plt.plot(DR_list, T_list)
    plt.title("Saturation Throughput vs. Data Rate")
    plt.xlabel("Data Rate")
    plt.ylabel("Saturation Throughput")
    plt.savefig("throughput_rate.png")
    # plt.show()