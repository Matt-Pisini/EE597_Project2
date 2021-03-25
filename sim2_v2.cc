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
    //std::string Data_Rate = "1Mb/s";      //data rate (Mbits/s)
    uint32_t minCw = 1;
    uint32_t maxCw = 1023;
    //CommandLine cmd;
    //cmd.AddValue("N", "Number of Tx Nodes/Devices", N);
   // cmd.AddValue("Data_Rate", "Data rate enter as string example: 5Mb/s", Data_Rate);
    //cmd.AddValue("minCw", "Minimum contention window size", minCw);
   // cmd.AddValue("maxCw", "Maximimum contention window size", maxCw);
   // cmd.Parse(argc, argv);

    //Config::SetDefault("ns3::Txop::CwMin", UintegerValue(minCw));
    //Config::SetDefault("ns3::Txop::CwMax", UintegerValue(maxCw));


    //Creating nodes for Tx and Rx
    NodeContainer wifiRxNode;
    wifiRxNode.Create(uint32_t(1));
    NodeContainer wifiTxNodes;
    wifiTxNodes.Create(N);


//matt
    //Setup wifi channel
    //Do we need to set propagation delay? Might be according to the 
    //research paper as well.    
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel",
                                "Speed", StringValue("1000000"));
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());


//matt
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

//will
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


//will    
    //Set up Mobility and Position
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    float rho = 1;
    float pi = 3.14159265;
    double theta = 0;
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

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer wifiRxInterface = address.Assign(rxDevice);
    Ipv4InterfaceContainer wifiTxInterface = address.Assign(txDevices);

    for(uint32_t i = 0; i < N; i++){
        OnOffHelper client ("ns3::UdpSocketFactory", Address(InetSocketAddress(wifiRxInterface.GetAddress(uint32_t(0)), uint32_t(9))));
        client.SetConstantRate(DataRate("5Mb/s"), uint32_t(512));
        ApplicationContainer clientApp = client.Install(wifiTxNodes.Get(i));
        clientApp.Start(Seconds(0.1));
        clientApp.Stop(Seconds(9.0));
    }

    // Build your applications and monitor throughtput:
    //UdpServerHelper and UdpClientHelper are meant to help facilitate client-server communication for UDP
    PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(wifiRxInterface.GetAddress(uint32_t(0)), uint32_t(9))));
    ApplicationContainer serverApp = sink.Install(wifiRxNode.Get(uint32_t(0)));
    serverApp.Start(Seconds(0.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    //run simulator and get throughput
    Ptr<PacketSink> sinkApp = DynamicCast<PacketSink> (serverApp.Get(uint32_t(0)));
    FlowMonitorHelper flowMon;
    Ptr<FlowMonitor> monitor = flowMon.InstallAll();

    Simulator::Stop(Seconds(15.0));
    Simulator::Run();

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowMon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    double endTrans = 9.0;
    double startTrans = 10.0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it= stats.begin(); it!= stats.end(); it++){
        if (endTrans < it->second.timeLastRxPacket.GetSeconds()){
            endTrans = it->second.timeLastRxPacket.GetSeconds();
        }
        if(startTrans > it->second.timeFirstRxPacket.GetSeconds()){
            startTrans = it->second.timeFirstRxPacket.GetSeconds();
        }
    }

    double totalBytes = sinkApp->GetTotalRx();
    float throughput = totalBytes*8.0/(endTrans - startTrans);
    throughput = throughput/1000;
    std::cout<<"throughtput: "<<throughput<<std::endl;

    Simulator::Destroy();

    return 0;
}
