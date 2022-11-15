#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <ns3/lorawan-module.h>
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

typedef enum
{
  LORAWAN_DR_CALC_METHOD_PER_INDEX = 0x00,
  LORAWAN_DR_CALC_METHOD_RANDOM_INDEX = 0x01,
  LORAWAN_DR_CALC_METHOD_FIXED_INDEX = 0x02,
} LoRaWANDataRateCalcMethodIndex;

class LoRaWANExampleTracing
{
public:
  LoRaWANExampleTracing ();
  void CaseRun (uint32_t nEndDevices,
               uint32_t nGateways,
               double discRadius,
               double totalTime,
               uint32_t usPacketSize,
               uint32_t usMaxBytes,
               double usDataPeriod,
               uint32_t usUnconfirmedDataNbRep,
               bool usConfirmedData,
               uint32_t dsPacketSize,
               bool dsDataGenerate,
               double dsDataExpMean,
               bool dsConfirmedData,
               bool verbose,
               bool stdcout,
               bool tracePhyTransmissions,
               bool tracePhyStates,
               bool traceMacPackets,
               bool traceMacStates,
               bool traceEdMsgs,
               bool traceNsDsMsgs,
               bool traceMisc,
               std::string phyTransmissionTraceCSVFileName,
               std::string phyStateTraceCSVFileName,
               std::string macPacketTraceCSVFileName,
               std::string macStateTraceCSVFileName,
               std::string edMsgTraceCSVFileName,
               std::string nsDSMsgTraceCSVFileName,
               std::string miscTraceCSVFileName,
               std::string nodesCSVFileName);

  void LogOutputLine (std::string output, std::string);

  std::ostringstream GeneratePhyTraceOutputLine (std::string traceSource, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, Ptr<const Packet> packet, bool insertNewLine = true);
  static void PhyTxBegin (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, Ptr<const Packet> packet);
  static void PhyTxEnd (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, Ptr<const Packet> packet);
  static void PhyTxDrop (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, Ptr<const Packet> packet);
  static void PhyRxBegin (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, Ptr<const Packet> packet);
  static void PhyRxEnd (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, Ptr<const Packet> packet, double lqi);
  static void PhyRxDrop (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, Ptr<const Packet> packet, LoRaWANPhyDropRxReason dropReason);

  std::ostringstream GenerateMacTraceOutputLine (std::string traceSource, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, Ptr<const Packet> packet, bool insertNewLine = true);
  static void MacTx (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, Ptr<const Packet> packet);
  static void MacTxOk(LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, Ptr<const Packet> packet);
  static void MacTxDrop(LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, Ptr<const Packet> packet);
  static void MacRx(LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, Ptr<const Packet> packet);
  static void MacRxDrop (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, Ptr<const Packet> packet);
  static void MacSentPkt (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, Ptr<const Packet> packet, uint8_t n_transmissions);

  static void PhyStateChangeNotification (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANPhy> phy, LoRaWANPhyEnumeration oldState, LoRaWANPhyEnumeration newState);
  static void MacStateChangeNotification (LoRaWANExampleTracing* example, Ptr<LoRaWANNetDevice> device, Ptr<LoRaWANMac> mac, LoRaWANMacState oldState, LoRaWANMacState newState);

  static void nrRW1SentTrace (LoRaWANExampleTracing* example, uint32_t oldValue, uint32_t newValue);
  static void nrRW2SentTrace (LoRaWANExampleTracing* example, uint32_t oldValue, uint32_t newValue);
  static void nrRW1MissedTrace (LoRaWANExampleTracing* example, uint32_t oldValue, uint32_t newValue);
  static void nrRW2MissedTrace (LoRaWANExampleTracing* example, uint32_t oldValue, uint32_t newValue);

  std::ostringstream GenerateEDMsgTraceOutputLine (std::string traceSource, uint32_t deviceAddress, uint8_t msgType, Ptr<const Packet> packet, bool insertNewLine = true);
  static void USMsgTransmittedTrace (LoRaWANExampleTracing* example, uint32_t deviceAddress, uint8_t msgType, Ptr<const Packet> packet);
  static void DSMsgReceivedTrace (LoRaWANExampleTracing* example, uint32_t deviceAddress, uint8_t msgType, Ptr<const Packet> packet, uint8_t rw);

  std::ostringstream GenerateNSDSMsgTraceOutputLine (std::string traceSource, uint32_t deviceAddress, uint8_t transmissionsRemaning, uint8_t msgType, Ptr<const Packet> packet, bool insertNewLine = true);
  static void DSMsgGeneratedTrace (LoRaWANExampleTracing* example, uint32_t, uint8_t, uint8_t, Ptr<const Packet>);
  static void DSMsgTransmittedTrace (LoRaWANExampleTracing* example, uint32_t, uint8_t, uint8_t, Ptr<const Packet>, uint8_t rw);
  static void DSMsgAckdTrace (LoRaWANExampleTracing* example, uint32_t, uint8_t, uint8_t, Ptr<const Packet>);
  static void DSMsgDroppedTrace (LoRaWANExampleTracing* example, uint32_t, uint8_t, uint8_t, Ptr<const Packet>);
  static void USMsgReceivedTrace (LoRaWANExampleTracing* example, uint32_t deviceAddress, uint8_t msgType, Ptr<const Packet> packet);

  constexpr static const uint8_t m_perPacketSize = 1 + 8 + 8 + 4; // 1B MAC header, 8B frame header, 8 byte payload and 4B MIC
  uint8_t CalculateDataRateIndexPER (Ptr<Application> endDeviceApp);
  uint8_t CalculateRandomDataRateIndex (Ptr<Application> endDeviceApp);
  uint8_t CalculateFixedDataRateIndex (Ptr<Application> endDeviceApp);

  void SelectDRCalculationMethod (LoRaWANDataRateCalcMethodIndex loRaWANDataRateCalcMethodIndex);
  void SetDRCalcPerLimit (double drCalcPerLimit);
  void SetDrCalcFixedDrIndex (uint8_t fixedDataRateIndex);

  void WriteMiscStatsToFile ();
private:
  uint32_t m_nEndDevices;
  uint32_t m_nGateways;
  double m_discRadius;
  double m_totalTime;

  LoRaWANDataRateCalcMethodIndex m_drCalcMethodIndex;
  double m_drCalcPerLimit;
  uint8_t m_fixedDataRateIndex;

  uint32_t m_usPacketSize;
  uint32_t m_usMaxBytes;
  double m_usDataPeriod; // <! Period between subsequent data transmission of end devices
  uint32_t m_usUnconfirmedDataNbRep;
  bool m_usConfirmdData;

  uint32_t m_dsPacketSize;
  bool m_dsDataGenerate;
  double m_dsDataExpMean;
  bool m_dsConfirmedData;

  bool m_verbose;
  bool m_stdcout;
  std::string m_phyTransmissionTraceCSVFileName;
  std::string m_phyStateTraceCSVFileName;
  std::string m_macPacketTraceCSVFileName;
  std::string m_macStateTraceCSVFileName;
  std::string m_edMsgTraceCSVFileName;
  std::string m_nsDSMsgTraceCSVFileName;
  std::string m_miscTraceCSVFileName;
  std::map<std::string, std::ofstream> output_streams;

  std::string m_nodesCSVFileName;

  uint32_t m_nrRW1Sent;
  uint32_t m_nrRW2Sent;
  uint32_t m_nrRW1Missed;
  uint32_t m_nrRW2Missed;

  std::string m_rate;
  std::string m_phyMode;
  uint32_t m_nodeSpeed;
  uint32_t m_periodicUpdateInterval;
  uint32_t m_settlingTime;
  double m_dataStart;
  uint32_t bytesTotal;
  uint32_t packetsReceived;
  bool m_printRoutes;

  private:
  void CreateNodes ();
  void SetupMobility ();
  void CreateDevices ();
  void SetupTracing (bool tracePhyTransmissions, bool tracePhyStates, bool traceMacPackets, bool traceMacStates, bool traceEdMsgs, bool traceNsDsMsgs, bool traceMisc);
  void InstallApplications ();
  void OutputNodesToFile ();
};

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

  // LoRa
    uint32_t randomSeed = 12345;
  uint32_t nEndDevices = 5;
  uint32_t nGateways = 4;
  double discRadius = 1000.0;
  double totalTime = 600.0;
  uint32_t nRuns = 1;
  uint32_t drCalcMethodIndex = 0;
  double drCalcPerLimit = 0.01;
  double drCalcFixedDRIndex = 0;
  uint32_t usPacketSize = 21;
  uint32_t usMaxBytes = 0;
  double usDataPeriod = 600.0;
  uint32_t usUnconfirmedDataNbRep = 1;
  bool usConfirmedData = false;
  uint32_t dsPacketSize = 21;
  bool dsDataGenerate = false;
  double dsDataExpMean = -1;
  bool dsConfirmedData = false;
  bool verbose = false;
  bool stdcout = false;
  bool tracePhyTransmissions = false;
  bool tracePhyStates = false;
  bool traceMacPackets = false;
  bool traceMacStates = false;
  bool traceEdMsgs = false;
  bool traceNsDsMsgs = false;
  bool traceMisc = false;
  uint32_t nodeSpeed = 10;
  uint32_t periodicUpdateInterval = 15;
  std::string outputFileNamePrefix = "output/LoRaWAN-example-tracing";
  std::string phyMode = "DsssRate11Mbps";
  std::string rate = "8kbps";
  uint32_t settlingTime = 6;
  double dataStart = 50.0;
  uint32_t printRoutingTable = 1;

  CommandLine cmd;
  cmd.AddValue ("standard",
                "OFDM-based Wi-Fi standard [11a]",
                standard);
  cmd.AddValue ("bw", "Bandwidth (consistent with standard, in MHz)", bw);
  cmd.AddValue ("txPower", "Transmit power (dBm)", pow);
  cmd.AddValue ("randomSeed", "Random seed used in experiments[Default:12345]", randomSeed);
  cmd.AddValue ("nEndDevices", "Number of LoRaWAN class A end devices nodes[Default:30]", nEndDevices);
  cmd.AddValue ("nGateways", "Number of LoRaWAN gateways [Default:1]", nGateways);
  cmd.AddValue ("discRadius", "The radius of the disc (in meters) in which end devices and gateways are placed[Default:5000.0]", discRadius);
  cmd.AddValue ("totalTime", "Simulation time for one run in Seconds[Default:100]", totalTime);
  cmd.AddValue ("nRuns", "Number of simulation runs[Default:100]", nRuns);
  cmd.AddValue ("drCalcMethodIndex", "Method to use for end device Data Rate calculation[Default:0]", drCalcMethodIndex);
  cmd.AddValue ("drCalcPerLimit", "PER limit to use when using the PER Data Rate Calculation method (use index = 0)[Default:0.01]", drCalcPerLimit);
  cmd.AddValue ("drCalcFixedDRIndex", "Fixed Data Rate index to assign when using the Fixed Data Rate Calculation method (use index = 2)[Default:0]", drCalcFixedDRIndex);
  cmd.AddValue ("usPacketSize", "Packet size used for generating US packets[Default:21]", usPacketSize);
  cmd.AddValue ("usMaxBytes", "Maximum number of bytes to be queued for transmission in the upstream direction, note this does not take into account retransmissions. 0 means no limit. [Default:0]", usMaxBytes);
  cmd.AddValue ("usDataPeriod", "Period between subsequent Upstream data transmissions from an end device[Default:600]", usDataPeriod);
  cmd.AddValue ("usUnconfirmedDataNbRep", "Number of repetitions for unconfirmed US data messages[Default:1]", usUnconfirmedDataNbRep);
  cmd.AddValue ("usConfirmedData", "0 for Unconfirmed Upstream Data MAC packets, 1 for Confirmed Upstream Data MAC Packets[Default:0]", usConfirmedData);
  cmd.AddValue ("dsPacketSize", "Packet size used for generating DS packets[Default:21]", dsPacketSize);
  cmd.AddValue ("dsDataGenerate", "Should NS generate DS traffic for end devices, note that Acks are always sent.[Default:0]", dsDataGenerate);
  cmd.AddValue ("dsDataExpMean", "Mean for the Exponential random variable for inter packet time for DS transmission for an end device[Default:10*usDataPeriod]", dsDataExpMean);
  cmd.AddValue ("dsConfirmedData", "0 for Unconfirmed Downstream Data MAC packets, 1 for Confirmed Downstream Data MAC Packets[Default:0]", dsConfirmedData);
  cmd.AddValue ("verbose", "turn on all log components[Default:0]", verbose);
  cmd.AddValue ("stdcout", "Print output to std::cout[Default:0]", stdcout);
  cmd.AddValue ("tracePhyTransmissions", "Trace PHY transmissions[Default:0]", tracePhyTransmissions);
  cmd.AddValue ("tracePhyStates", "Trace PHY states[Default:0]", tracePhyStates);
  cmd.AddValue ("traceMacPackets", "Trace MAC packets[Default:0]", traceMacPackets);
  cmd.AddValue ("traceMacStates", "Trace MAC states[Default:0]", traceMacStates);
  cmd.AddValue ("traceEDMsgs", "Trace messages on end devices[Default:0]", traceEdMsgs);
  cmd.AddValue ("traceNSDSMsgs", "Trace NS downstream messages[Default:0]", traceNsDsMsgs);
  cmd.AddValue ("traceMisc", "Trace miscellanous stats[Default:0]", traceMisc);
  cmd.AddValue ("outputFileNamePrefix", "The prefix for the names of the output files[Default:output/LoRaWAN-example-tracing]", outputFileNamePrefix);
  cmd.AddValue ("phyMode", "Wifi Phy mode[Default:DsssRate11Mbps]", phyMode);
  cmd.AddValue ("rate", "CBR traffic rate[Default:8kbps]", rate);
  cmd.AddValue ("nodeSpeed", "Node speed in RandomWayPoint model[Default:10]", nodeSpeed);
  cmd.AddValue ("periodicUpdateInterval", "Periodic Interval Time[Default=15]", periodicUpdateInterval);
  cmd.AddValue ("settlingTime", "Settling Time before sending out an update for changed metric[Default=6]", settlingTime);
  cmd.AddValue ("dataStart", "Time at which nodes start to transmit data[Default=50.0]", dataStart);
  cmd.AddValue ("printRoutingTable", "print routing table for nodes[Default:1]", printRoutingTable);  
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
  wifiNodes.Create (2);
  spectrumAnalyzerNodes.Create (1);
  allNodes.Add (wifiNodes);
  allNodes.Add (spectrumAnalyzerNodes);
  wifiApNode.Add (wifiNodes.Get (0));
  wifiStaNode.Add (wifiNodes.Get (1));

  /* LoRa */
  NodeContainer m_endDeviceNodes;
  NodeContainer m_gatewayNodes;
  NodeContainer m_allNodes;
  
  NetDeviceContainer m_EDDevices;
  NetDeviceContainer m_GWDevices;

  // connecting WiFi (wifiApNode) and LoRa (m_gatewayNodes)
  conn.Add(wifiApNode);
  conn.Add(m_gatewayNodes);

  
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