#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
//#include "ns3/lorawan-module.h"
#include "ns3/wifi-module.h"
#include <ns3/log.h>
#include <ns3/lora-zigbee-module.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/packet.h>
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include <ns3/netanim-module.h>
#include "ns3/spectrum-helper.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/spectrum-analyzer-helper.h"
#include "ns3/spectrum-channel.h"
#include "ns3/mobility-helper.h"
#include <iostream>

using namespace ns3; //Используется пространство имен ns‑3

NS_LOG_COMPONENT_DEFINE ("PTP-WL"); //Определяется компонент журналирования

void
SendPacket (Ptr<NetDevice> sourceDevice, Address& destination)
{
  Ptr<Packet> pkt = Create<Packet> (100);  // dummy bytes of data
  sourceDevice->Send (pkt, destination, 0);
}

int main (int argc, char *argv[])
{
  std::string standard = "11a";
  int bw = 20; //Bandwidth
  double pow = 23; //dBm
  CommandLine cmd;
  cmd.AddValue ("standard",
                "OFDM-based Wi-Fi standard [11a]",
                standard);
  cmd.AddValue ("bw", "Bandwidth (consistent with standard, in MHz)", bw);
  cmd.AddValue ("txPower", "Transmit power (dBm)", pow);
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


  Time::SetResolution (Time::NS);
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    

  /* nodes */
  NodeContainer conn;
  NodeContainer allNodes;
  NodeContainer wifiNodes;
  NodeContainer spectrumAnalyzerNodes;
  NodeContainer wifiStaNode;
  NodeContainer wifiApNode;
  conn.Add(wifiApNode);
  wifiNodes.Create (2);
  spectrumAnalyzerNodes.Create (1);
  allNodes.Add (wifiNodes);
  allNodes.Add (spectrumAnalyzerNodes);
  wifiApNode.Add (wifiNodes.Get (0));
  wifiStaNode.Add (wifiNodes.Get (1));
  
  // Создание экземпляра PointToPointHelper и установка связанных атрибутов так, чтобы на устройствах, 
  // созданных с помощью помощника, получить передачу со скоростью 5 МБ/с и задержкой 5 наносекунд в канале.

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ns"));
  NetDeviceContainer devices;
  devices = pointToPoint.Install (conn);

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
 
  // Создаем стек протокол
  InternetStackHelper stack;
  stack.Install (allNodes);
  
  // Далее нужно назначить IP-адресс
  Ipv4AddressHelper address;
  address.SetBase ("192.168.0.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);
  
  /* Создаем экземпляр сервера на одном из узлов с устройством
  и клиенте на узле, имеющем только двухточечное устройство */
  UdpEchoServerHelper echoServer (4445); //Настройка эха сервера
  ApplicationContainer serverApps = echoServer.Install (allNodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));
  
  /* Мы передаем необходимые атрибуты в конструктор UdpEchoClientHelper. 
  Мы говорим клиенту отправлять пакеты на сервер.
  Мы устанавливаем клиент на первой точке (nodes 0). */
  UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 4445);
  echoClient.SetAttribute("MaxPackets", UintegerValue(1));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue(1024));
  
  ApplicationContainer clientApps = echoClient.Install (conn.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
 
  Simulator::Run ();
  Simulator::Stop ();
}