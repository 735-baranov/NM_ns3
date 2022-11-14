#include <ns3/log.h>
#include <ns3/core-module.h>
#include <ns3/lorawan-module.h>
#include <ns3/lora-zigbee-module.h>
#include <ns3/wifi-module.h>
#include "ns3/spectrum-helper.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/spectrum-analyzer-helper.h"
#include "ns3/spectrum-channel.h"
#include "ns3/mobility-helper.h"
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/packet.h>
#include <ns3/applications-module.h>
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include <ns3/netanim-module.h>
#include <ns3/network-module.h>
#include <ns3/ipv4-address.h>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("wl");


static void DataIndicationLora (McpsDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("Индикация данных: полученный пакет для Lora размером  " << p->GetSize ());
}
static void DataConfirmLora(McpsDataConfirmParams params)
{
  NS_LOG_UNCOND ("Статус подтверждения данных LoraMcps = " << params.m_status);
}

// static void StateChangeNotification (std::string context, Time now, LrWpanPhyEnumeration oldState, LrWpanPhyEnumeration newState)
// {
//   NS_LOG_UNCOND (context << " изменение состояния в " << now.GetSeconds ()
//                          << " от " << LrWpanHelper::LrWpanPhyEnumerationPrinter (oldState)
//                          << " к " << LrWpanHelper::LrWpanPhyEnumerationPrinter (newState));
// }

void //for wifi
SendPacket (Ptr<NetDevice> sourceDevice, Address& destination)
{
  Ptr<Packet> pkt = Create<Packet> (100);  // dummy bytes of data
  sourceDevice->Send (pkt, destination, 0);
}

void //for lora
ReceivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  uint64_t bytes = 0;
  while ((packet = socket->Recv ()))
    {
      bytes += packet->GetSize ();
    }

  std::cout << "SOCKET received " << bytes << " bytes" << std::endl;
}

Ptr<Socket> //for lora
SetupPacketReceive (Ptr<Node> node)
{
  TypeId tid = TypeId::LookupByName ("ns3::PacketSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  sink->Bind ();
  sink->SetRecvCallback (MakeCallback (&ReceivePacket));
  return sink;
}

int
main (int argc, char *argv[])
{

  //wifi
  std::string standard = "11a"; // пока что только 11а
  
  int bw = 20;
  double pow = 23; //dBm
  bool verbose = false;
  CommandLine cmd;
  cmd.AddValue ("standard",
                "OFDM-based Wi-Fi standard [11a, 11_10MHZ, 11_5MHZ, 11n_2_4GHZ, 11n_5GHZ, 11ac, 11ax_2_4GHZ, 11ax_5GHZ]",
                standard);
  cmd.AddValue ("bw", "Bandwidth (consistent with standard, in MHz)", bw);
  cmd.AddValue ("txPower", "Transmit power (dBm)", pow);
  cmd.AddValue ("verbose", "Display log messages for WifiSpectrumValueHelper and SpectrumWifiPhy", verbose);
  cmd.Parse (argc,argv);

  WifiHelper wifi;
  Ssid ssid;
  std::string dataRate;
  int freq;
  Time dataStartTime = MicroSeconds (800); // leaving enough time for beacon and association procedure
  Time dataDuration = MicroSeconds (300); // leaving enough time for data transfer (+ acknowledgment)

  if (standard == "11a")
    {
      wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
      ssid = Ssid ("ns380211a");
      dataRate = "OfdmRate6Mbps";
      freq = 5180;
      if (bw != 20)
        {
          std::cout << "Bandwidth is not compatible with standard" << std::endl;
          return 1;
        }
    }
  else
    {
      std::cout << "Unknown OFDM standard (please refer to the listed possible values)" << std::endl;
      return 1;
    }

  /* nodes and positions */
  NodeContainer wifiNodes;
  NodeContainer spectrumAnalyzerNodes;
  NodeContainer allNodes;
  wifiNodes.Create (2);
  spectrumAnalyzerNodes.Create (1);
  allNodes.Add (wifiNodes);
  allNodes.Add (spectrumAnalyzerNodes);
  
  NodeContainer wifiStaNode;
  NodeContainer wifiApNode;
  wifiApNode.Add (wifiNodes.Get (0));
  wifiStaNode.Add (wifiNodes.Get (1));

  /* channel and propagation */
  SpectrumChannelHelper channelHelper = SpectrumChannelHelper::Default ();
  channelHelper.SetChannel ("ns3::MultiModelSpectrumChannel");
  // constant path loss added just to show capability to set different propagation loss models
  // FriisSpectrumPropagationLossModel already added by default in SpectrumChannelHelper
  channelHelper.AddSpectrumPropagationLoss ("ns3::ConstantSpectrumPropagationLossModel");
  Ptr<SpectrumChannel> channel = channelHelper.Create ();

  /* Wi-Fi transmitter setup */

  SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default ();
  spectrumPhy.SetChannel (channel);
  spectrumPhy.SetErrorRateModel ("ns3::NistErrorRateModel");
  spectrumPhy.Set ("Frequency", UintegerValue (freq));
  spectrumPhy.Set ("ChannelWidth", UintegerValue (bw));
  spectrumPhy.Set ("TxPowerStart", DoubleValue (pow)); // dBm
  spectrumPhy.Set ("TxPowerEnd", DoubleValue (pow));

  WifiMacHelper mac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", StringValue (dataRate),
                                "ControlMode", StringValue (dataRate));

  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
  NetDeviceContainer staDevice = wifi.Install (spectrumPhy, mac, wifiStaNode);
  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false)); // so as to be sure that first beacon arrives quickly
  NetDeviceContainer apDevice = wifi.Install (spectrumPhy, mac, wifiApNode);

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> nodePositionList = CreateObject<ListPositionAllocator> ();
  nodePositionList->Add (Vector (0.0, 1.0, 0.0)); // AP
  nodePositionList->Add (Vector (1.0, 0.0, 0.0)); // STA
  nodePositionList->Add (Vector (0.0, 0.0, 0.0));  // Spectrum Analyzer
  mobility.SetPositionAllocator (nodePositionList);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (allNodes);

  /* Need to send data packet because beacon and association frames shall be sent using lowest rate */
  // Send one data packet (this packet is sent using data rate / MCS defined above) once association is done (otherwise dropped)
  Simulator::Schedule (dataStartTime, &SendPacket, apDevice.Get (0), staDevice.Get (0)->GetAddress ());

  /* frequency range for spectrum analyzer */
  std::vector<double> freqs;
  int margin = 2; //1MHz margin on each side
  int band = (bw + margin);
  for (int i = 0; i < (4 * 10 * band); ++i) //conversion to 100kHz scale
    {
      freqs.push_back (i * 1e5 + (freq - 2 * band) * 1e6);
    }
  Ptr<SpectrumModel> spectrumAnalyzerFreqModel = Create<SpectrumModel> (freqs);

  /* spectrum analyzer setup */
  SpectrumAnalyzerHelper spectrumAnalyzerHelper;
  spectrumAnalyzerHelper.SetChannel (channel);
  spectrumAnalyzerHelper.SetRxSpectrumModel (spectrumAnalyzerFreqModel);
  spectrumAnalyzerHelper.SetPhyAttribute ("Resolution", TimeValue (MicroSeconds (4))); //enough resolution to distinguish OFDM symbols (default 1ms too long even for PPDUs)
  // std::ostringstream ossFileName;
  // ossFileName << "spectrum-analyzer-wifi-lora-" << standard << "-" << bw << "MHz";
  // spectrumAnalyzerHelper.EnableAsciiAll (ossFileName.str ());
  // NetDeviceContainer spectrumAnalyzerDevices = spectrumAnalyzerHelper.Install (spectrumAnalyzerNodes);




// lora 
  NodeContainer endDeviceNodes;
  NodeContainer gatewayNodes;
  NodeContainer allNodes;

  endDeviceNodes.Create (1);
  gatewayNodes.Create (1);
  allNodes.Add (endDeviceNodes);
  allNodes.Add (gatewayNodes);

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> nodePositionList = CreateObject<ListPositionAllocator> ();
  nodePositionList->Add (Vector (5.0, 5.0, 0.0));  // end device1
  // nodePositionList->Add (Vector (-5.0, 5.0, 0.0));  // end device2
  // nodePositionList->Add (Vector (5.0, -5.0, 0.0));  // end device3
  // nodePositionList->Add (Vector (-5.0, -5.0, 0.0));  // end device4
  nodePositionList->Add (Vector (0.0, 0.0, 0.0));  // gateway
  mobility.SetPositionAllocator (nodePositionList);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (allNodes);

  LoRaWANHelper lorawanHelper;
  NetDeviceContainer lorawanEDDevices = lorawanHelper.Install (endDeviceNodes);

  lorawanHelper.SetDeviceType (LORAWAN_DT_GATEWAY);
  NetDeviceContainer lorawanGWDevices = lorawanHelper.Install (gatewayNodes);

// Note to self: PacketSocketHelper::Install adds a PacketSocketFactory
  // object as an aggregate object to each of the nodes in the NodeContainer
  PacketSocketHelper packetSocket;
  packetSocket.Install (endDeviceNodes);
  packetSocket.Install (gatewayNodes);

  // Not sure what this does
  PacketSocketAddress socket;
  socket.SetSingleDevice (lorawanEDDevices.Get (0)->GetIfIndex ()); // Set the address to match only a specified NetDevice...
  // socket.SetPhysicalAddress (lorawanGWDevices.Get (0)->GetAddress ()); // Set destination address
  //socket.SetProtocol (1); // Set the protocol

  // OnOffHelper onoff ("ns3::PacketSocketFactory", Address (socket));
  // onoff.SetAttribute ("OnTime", StringValue ("ns3::ExponentialRandomVariable[Mean=100]"));
  // onoff.SetAttribute ("OffTime", StringValue ("ns3::ExponentialRandomVariable[Mean=10]"));
  // //onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("0.4Mbps")));
  // //onoff.SetAttribute ("DataRate", DataRateValue (DataRate (3*8))); // 3 bytes per second
  // onoff.SetAttribute ("DataRate", DataRateValue (DataRate (6*8))); // 3 bytes per second
  // onoff.SetAttribute ("PacketSize", UintegerValue (30));

  // ApplicationContainer apps = onoff.Install (endDeviceNodes.Get (0));
  // apps.Start (Seconds (0.0));
  // apps.Stop (Seconds (100));

  Ptr<Socket> recvSink = SetupPacketReceive (gatewayNodes.Get (0));

  //McpsDataConfirmCallback

  Simulator::Run ();

  Simulator::Destroy ();

  // std::cout << "Simulation done!" << std::endl;
  // std::cout << "See spectrum analyzer output file: " << ossFileName.str () << ".tr" << std::endl;
  // std::cout << "To generate plot simply execute the following command: gnuplot " << ossFileName.str () << ".plt" << std::endl;

  return 0;
}