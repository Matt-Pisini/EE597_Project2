//#include <fstream>
//#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/csma-module.h"
#include <math.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Sim2ScenarioA");

int main(int argc, char *argv[]){
    //Reading command line arguments
    uint32_t N = 7;
    uint64_t Data_Rate = 5;      //data rate (Mbits/s)
    uint32_t minCw = 1;
    uint32_t maxCw = 1023;
    char CASE = 'A';
    CommandLine cmd;
    cmd.AddValue("N", "Number of Tx Nodes/Devices", N);
    cmd.AddValue("Data_Rate", "Data rate in units of Mb/s", Data_Rate);
    cmd.AddValue("CASE", "Case A Cwnd size 1-1023 Case B Cwnd size 63-127", CASE);
    cmd.Parse(argc, argv);
   
    //Change Cwnd max and min if cae B is selected. Default is Case A.
    if(CASE == 'B' || CASE =='b'){
	minCw = 63;
	maxCw = 127;
    }

    //multiple Data rate by 1000000 to convert to Kb/s
    Data_Rate *= 1000000;

    //Creating nodes for Tx and Rx
    NodeContainer wifiRxNode;
    wifiRxNode.Create(uint32_t(1));
    NodeContainer wifiTxNodes;
    wifiTxNodes.Create(N);


    //Setup wifi channel
    //Do we need to set propagation delay? Might be according to the 
    //research paper as well.    
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel",
                                "Speed", StringValue("1000000"));
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());


    //Setup wifi Module
    //Need to look into wifi-mac.cc or wifi-phy.cc for configuration
    //parameters to change according to research paper.    
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue("DsssRate1Mbps"),
                                "ControlMode", StringValue("DsssRate1Mbps"));
    WifiMacHelper mac;
    Ssid ssid = Ssid("ns3-80211b");
    mac.SetType("ns3::AdhocWifiMac", "Ssid", SsidValue(ssid));


    //Install NetDevices (i.e. abstraction for NIC to work with channel)
    NetDeviceContainer rxDevice;
    rxDevice = wifi.Install(phy, mac, wifiRxNode);
    NetDeviceContainer txDevices;
    txDevices = wifi.Install(phy, mac, wifiTxNodes);

    //Set up backoff window size
    Ptr<NetDevice> dev = wifiRxNode.Get(0)->GetDevice(0);
    Ptr<WifiNetDevice> wifi_dev = DynamicCast<WifiNetDevice>(dev);
    Ptr<WifiMac> wifi_mac = wifi_dev->GetMac();
    PointerValue ptr;
    Ptr<Txop> dca;
    wifi_mac->GetAttribute("Txop", ptr);
    dca = ptr.Get<Txop>();
    dca->SetMinCw(minCw);
    dca->SetMaxCw(maxCw);


    //Set up Mobility and Position
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    float rho = 1;
    float pi = 3.14159265;
    double theta = 0;
    //Placing Transmiters in an equal distance circle around receiver
    for(uint32_t i = 0; i <N; i++){
        theta = i*2*pi/N;
        positionAlloc->Add(Vector(rho*cos(theta),rho*sin(theta),0.0));
    }
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(wifiRxNode);
    mobility.Install(wifiTxNodes);

    //Install network stack on nodes
    //EXPLANATION
    //We use this function to aggregate other objects for IP/TCP/UDP functionality to our existing nodes. 
    //This includes protocols like ARP, IPv4, neighbor discovery, and more. Essentially our nodes are basic
    //until we add all this internet related functionality to them.    
    
    InternetStackHelper stack;
    stack.Install(wifiRxNode);
    stack.Install(wifiTxNodes);

    //Configure IP addresses and internet interfaces:
    //EXPLANATION
    //Helper class that is a simple IPv4 address generator.
    
    Ipv4AddressHelper address;
    //Automatically assign IP addresses to the nodes 
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer wifiRxInterface = address.Assign(rxDevice);
    Ipv4InterfaceContainer wifiTxInterface = address.Assign(txDevices);
    
    //Setting each transmitter node to transmit at time .1 seconds from when simulation starts
    //They will transmit to the receivers address and port until 9 seconds
    for(uint32_t i = 0; i < N; i++){
        OnOffHelper client ("ns3::UdpSocketFactory", Address(InetSocketAddress(wifiRxInterface.GetAddress(uint32_t(0)), uint32_t(9))));
        client.SetConstantRate(DataRate(Data_Rate), uint32_t(512));
        ApplicationContainer clientApp = client.Install(wifiTxNodes.Get(i));
        clientApp.Start(Seconds(0.1));
        clientApp.Stop(Seconds(9.0));
    }

    // Build your applications and monitor throughtput:
    //UdpServerHelper and UdpClientHelper are meant to help facilitate client-server communication for UDP
    PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(wifiRxInterface.GetAddress(uint32_t(0)), uint32_t(9))));
    ApplicationContainer serverApp = sink.Install(wifiRxNode.Get(uint32_t(0)));
    //The receiver starts at time  0 seconds of when the simulation starts
    serverApp.Start(Seconds(0.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    //run simulator and get throughput
    //Using a flow monitor to be able to keep track of the data that is transmitted and evaluate throughput
    Ptr<PacketSink> sinkApp = DynamicCast<PacketSink> (serverApp.Get(uint32_t(0)));
    FlowMonitorHelper flowMon;
    Ptr<FlowMonitor> monitor = flowMon.InstallAll();
    //Set simulator to run for 15 seconds so that all transmitters have time to transmit for their 9 seconds
    Simulator::Stop(Seconds(15.0));
    Simulator::Run();
    
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowMon.GetClassifier());
    //maping the data statistics of each flow to the flow id (transmitter to receiver)
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    double endTrans = 9.0;
    double startTrans = 10.0;
    //uint64_t arrTxBytes[N];
    //uint64_t totalTxBytes;
    //int j = 0;

    //Checking the time of the first byte received from one of the transmitters and the time of the last byte received
    for (auto i : stats){
        if (endTrans < i.second.timeLastRxPacket.GetSeconds()){
            endTrans = i.second.timeLastRxPacket.GetSeconds();
        }
        if(startTrans > i.second.timeFirstRxPacket.GetSeconds()){
            startTrans = i.second.timeFirstRxPacket.GetSeconds();
        }
	//arrTxBytes[j] = i.second.txBytes;
	//totalTxBytes += arrTxBytes[j];
	//j+=1;
    }
    
    //Total amount of bits received
    double totalRxBytes = sinkApp->GetTotalRx();
    //throughput is bits/total time
    float throughput = totalRxBytes*8.0/(endTrans - startTrans);
    throughput = throughput/1000;//calculated in Kbps
    std::cout<<"Throughput (Kbps):\t"<<"Throughput-per-Node (Kbps)\t"<<"Number of Nodes\t\t"<<"Data Rate (Mbps)"<<std::endl; 
    std::cout<<"\t"<<throughput<<"\t\t\t"<<throughput/(double)N << "\t\t\t\t"<<N <<"\t\t\t"<< Data_Rate/1000000 <<std::endl;
    Simulator::Destroy();

    return 0;
}
