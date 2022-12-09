#include "ns3/log.h"
#include "ns3/command-line.h"
#include "ns3/config-store.h"
#include "ns3/config.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/ssid.h"
#include "ns3/spectrum-helper.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/spectrum-analyzer-helper.h"
#include "ns3/spectrum-channel.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-net-device.h"
#include "ns3/inet-socket-address.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WM");

void
SendPacket (Ptr<NetDevice> sourceDevice, Address& destination)
{
  Ptr<Packet> pkt = Create<Packet> (100);  // dummy bytes of data
  sourceDevice->Send (pkt, destination, 0); 
} 

int main (int argc, char *argv[])
{
    WifiHelper wifi;
    wifi.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
    Time dataStartTime = MicroSeconds (4700); // leaving enough time for beacon and association procedure
    Time dataDuration = MicroSeconds (400); // leaving enough time for data transfer (+ acknowledgment)

    NodeContainer wifiStaNode;
    wifiStaNode.Create (1);   // Create 1 station node objects
    NodeContainer wifiApNode;
    wifiApNode.Create (1);   // Create 1 access point node object

    // Create a channel helper and phy helper, and then create the channel
    SpectrumChannelHelper channelHepler = SpectrumChannelHelper::Default();
    channelHepler.SetChannel ("ns3::MultiModelSpectrumChannel");
    // constant path loss added just to show capability to set different propagation loss models
    // FriisSpectrumPropagationLossModel already added by default in SpectrumChannelHelper
    channelHepler.AddSpectrumPropagationLoss ("ns3::ConstantSpectrumPropagationLossModel");
    Ptr<SpectrumChannel> channel = channelHepler.Create ();
    
    SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default ();

    // Create a WifiMacHelper, which is reused across STA and AP configurations

    WifiMacHelper mac;
    Ssid ssid = Ssid ("ns-3-ssid");

    mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
    NetDeviceContainer wifiStaDevices = wifi.Install (spectrumPhy, mac, wifiStaNode);
    mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false)); // so as to be sure that first beacon arrives quickly
    NetDeviceContainer wifiApDevice = wifi.Install (spectrumPhy, mac, wifiApNode);

    Simulator::Schedule (dataStartTime, &SendPacket, wifiApDevice.Get (0), wifiStaDevices.Get (0)->GetAddress ());
    Simulator::Schedule (dataStartTime, &SendPacket, wifiStaDevices.Get (0), wifiApDevice.Get (0)->GetAddress ());    

    Simulator::Stop (dataStartTime + dataDuration);
    Simulator::Destroy ();

    std::cout << "Simulation done!" << std::endl;

    return 0;
}