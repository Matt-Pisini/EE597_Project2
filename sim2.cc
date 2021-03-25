#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/csma-module.h"
#include "ns3/txop.h"
#include "ns3/wifi-net-device.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/on-off-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/flow-monitor-helper.h"

/* NEED TO DO:
- Write a function that calculates throughput 
    - needs to be done for each node
    - I beleive within application container there are attributes that show this
- Make sure cmd line inputs work so we can input in N and DATA_RATE
- Make graphs of above ^

*/
#define END_SIMULATION 20.0 //total simulation time
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Sim2ScenarioA");

int main(int argc, char *argv[]){
    //Reading command line arguments
    uint32_t N = 3;          //Number of nodes in network
    uint64_t DATA_RATE = 1;      //data rate (Mbits/s)
    uint32_t minCw = 1;
    uint32_t maxCw = 1;
    CommandLine cmd (__FILE__);
    cmd.AddValue("N", "Number of Tx Nodes/Devices", N);
    cmd.AddValue("DATA_RATE", "Data Rate in Mbits/s", DATA_RATE);
    cmd.AddValue("minCw", "Minimum contention window size", minCw);
    cmd.AddValue("maxCw", "Maximum contention window size", maxCw);
    cmd.Parse(argc, argv);


    //Creating nodes for Tx and Rx
    NodeContainer wifiRxNode;
    wifiRxNode.Create(1);
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
                                "DataMode", StringValue("DsssRate1Mbps"));
    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::AdhocWifiMac", "Ssid", SsidValue(ssid));


    //Install NetDevices (i.e. abstraction for NIC to work with channel)
    NetDeviceContainer rxDevice;
    rxDevice = wifi.Install(phy, mac, wifiRxNode);
    NetDeviceContainer txDevices;
    txDevices = wifi.Install(phy, mac, wifiTxNodes);

    //will
    //Set up backoff window
    Ptr<NetDevice> dev = wifiRxNode.Get (0) -> GetDevice (0);
    Ptr<WifiNetDevice> wifi_dev = DynamicCast<WifiNetDevice> (dev);
    Ptr<WifiMac> wifi_mac = wifi_dev->GetMac ();
    PointerValue ptr;
    Ptr<Txop> dca;
    wifi_mac->GetAttribute ("Txop", ptr);
    dca = ptr.Get<Txop> ();
    dca-> SetMinCw(minCw);
    dca-> SetMaxCw(maxCw);

    //will
    //set up mobility and position
    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0.0, 0.0, 0.0));
    float rho = 1;
    float pi = 3.14159265;
    for (uint32_t i=0;i<N;i++){
        double theta = i*2*pi/N;
        positionAlloc->Add (Vector (rho*cos(theta), rho*sin(theta), 0.0));
    }
    mobility.SetPositionAllocator (positionAlloc);
    mobility.Install (wifiRxNode);
    mobility.Install (wifiTxNodes);

    //Install network stack on nodes
    /*********** EXPLANATION ***********
    We use this function to aggregate other objects for IP/TCP/UDP functionality to our existing nodes. 
    This includes protocols like ARP, IPv4, neighbor discovery, and more. Essentially our nodes are basic
    until we add all this internet related functionality to them.
    ***********************************/
    InternetStackHelper stack;
    stack.Install(wifiRxNode);
    stack.Install(wifiTxNodes);

    //Configure IP addresses and internet interfaces:
    /*********** EXPLANATION ***********
    Helper class that is a simple IPv4 address generator.
    ***********************************/
    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0","255.255.255.0"); //sets the base network IP and mask in which we allocate IP addresses to nodes

    Ipv4InterfaceContainer wifiInterfaces; //this object is a list of (Ptr<ipv4>,interface_index) pairs for all NetDevices
    wifiInterfaces = address.Assign(txDevices); //assign function allocates IP addresses to all the nodes in the NetDeviceContainer vector
    Ipv4InterfaceContainer wifiRxInterface;
    address.SetBase("10.0.1.0","255.255.255.0"); //sets the base network IP and mask in which we allocate IP addresses to nodes
    wifiRxInterface = address.Assign(rxDevice); //Not sure if I can just assign this without storing it in Ipv4InterfaceContainer vector... How/why is this vector used?


    // Build your applications and monitor throughtput:
    //UdpServerHelper and UdpClientHelper are meant to help facilitate client-server communication for UDP
    uint16_t udp_server_port = 7000; //port that the server will listen on
    UdpServerHelper server(udp_server_port); //we need to add the port that the server will listen on for incoming packets
    ApplicationContainer serverApp = server.Install(wifiRxNode); //holds vector of Application pointers. Install() creates one UDP application on each of input nodes from NodeContainer.
    std::string thing = "ns3::UdpSocketFactory";
    OnOffHelper client (thing,InetSocketAddress(wifiRxInterface.GetAddress(0),udp_server_port)); //makes it easier to work with OnOffApplications. UdpSocketFactory is API to create UDP socket instances sending to addr specified.
    // UdpClientHelper client(InetSocketAddress(wifiRxInterface.GetAddress(0),udp_server_port),udp_server_port); //address of remote UDP server
    ApplicationContainer clientApp = client.Install(wifiTxNodes); //install OnOffApplication on each node of input as specified by OnOffHelper. Holds vector of Application pointers. 
    client.SetConstantRate(DataRate(DATA_RATE*8*1000000), 512); //use OnOffHelper to set data rate (global variable we set) and packet size (which is default 512)
    client.SetAttribute("MaxPackets", 1);
    uint64_t totalPacketsThroughAP = DynamicCast<UdpServer> (serverApp.Get (0))->GetReceived ();
    // uint64_t totalPacketsSent = 0;
    // for( uint32_t i = 0; i < N; i++)
    // {
    //     totalPacketsSent += DynamicCast<UdpClient> (clientApp.Get (i))->GetTotalTx ();
    // }
    // double sat_throughput = (double) totalPacketsThroughAP * 512 / (double) totalPacketsSent;
    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    flowMonitor->SerializeToXmlFile("NameOfFile.xml", true, true);

    std::cout << totalPacketsThroughAP<< std::endl;

    //Set total simulation time
    clientApp.Start( Seconds(0.0) ); //arranges for all applications in this container (i.e. all nodes) to start at specified time
    clientApp.Stop( Seconds(END_SIMULATION) ); //arranges for all applications in this container (i.e. all nodes) to stop at specified time. Need to play around with this.
    serverApp.Start( Seconds(0.0) );
    serverApp.Stop( Seconds(END_SIMULATION) );

    //run simulation
    Simulator::Stop (Seconds (END_SIMULATION));
    Simulator::Run();
    Simulator::Destroy ();
    return 0;
}