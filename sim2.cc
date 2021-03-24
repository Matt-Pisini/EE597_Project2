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


}