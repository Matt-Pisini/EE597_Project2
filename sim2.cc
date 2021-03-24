#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/csma-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Sim2ScenarioA");

int main(int argc, char *argv[]){
    //Reading command line arguments
    int N = 3;
    CommandLine cmd (__FILE__);
    cmd.AddValue("N", "Number of Tx Nodes/Devices", N);
    cmd.Parse(argc, argv);


    //Creating nodes for Tx and Rx
    NodeContainer wifiRxNode;
    wifiRxNode.Create(1);
    NodeContainer wifiTxNodes;
    wifiTxNodes.Create(N);


    //Setup wifi channel
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel",
                                "Speed", StringValue("1000000"));
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());


    //Setup wifi Module
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiMangager",
                                "DataMode", StringValue("DsssRate1Mbps"));
    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::AdhocWifiMac", "Ssid", SsidValue(ssid));


    //Install NetDevices
    NetDeviceContainer rxDevice;
    rxDevice = wifi.Install(phy, mac, wifiRxNode);
    NetDeviceContainer txDevices;
    txDevices = wifi.Install(phy, mac, wifiTxNodes);

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

    //set up mobility and position
    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0.0, 0.0, 0.0));
    float rho = 1;
    float pi = 3.14159265;
    for (uint32_t i=0;i<nWifi;i++){
        double theta = i*2*pi/nWifi;
        positionAlloc->Add (Vector (rho*cos(theta), rho*sin(theta), 0.0));
    }
    mobility.SetPositionAllocator (positionAlloc);
    mobility.Install (wifiRxNode);
    mobility.Install (wifiTxNodes);


    //Install network stack on nodes
    InternetStackHelper stack;
    stack.Install(your nodes);

    //Configure IP addresses and internet interfaces:
    Ipv4AddressHelper address;
    Address.setBase(…);
    Ipv4InterfaceContainer wifiInterfaces;
    wifiInterface = address.Assign (your devices);

    // Build your applications and monitor throughtput:
    UdpServerHelper server(…);
    OnOffHelper client (…);
    UdpClientHelper client(…);
    ApplicationContainer serverApp = server.Install(your nodes);
    uint64_t totalPacketsThroughAP = DynamicCast<UdpServer> (serverApp.Get (0))->GetReceived ();

    //testing my git

}