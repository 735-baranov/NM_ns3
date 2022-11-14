#include <ns3/core-module.h>
#include <ns3/lora-zigbee-helper.h>
#include <ns3/network-module.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/lora-zigbee-mac.h>
#include <ns3/lora-zigbee-net-device.h>
// #include <ns3/lr-wpan-mac.h>
// #include <ns3/lr-wpan-net-device.h>
// #include <ns3/lr-wpan-helper.h>
#include <ns3/constant-position-mobility-model.h>
#include <iostream>
#include <iomanip>
#include <ns3/node.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>
#include <ns3/log.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lora-zigbee-example");
/*
static void DataIndicationZigbee (McpsDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("Индикация данных: полученный пакет для Zigbee размером  " << p->GetSize ());
}

static void DataConfirmZigbee (McpsDataConfirmParams params)
{
  NS_LOG_UNCOND ("Статус подтверждения данных ZigbeeMcps = " << params.m_status);
}

static void DataIndicationLora (LoRaWANDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("Индикация данных: полученный пакет для LoraWAN размером " << p->GetSize ());
}

static void DataConfirmLora (LoRaWANDataConfirmParams params)
{
  NS_LOG_UNCOND ("Статус подтверждения данных LoRaWANMcps = " << params.m_status);
}

static void StateChangeNotificationZigbee (std::string context, Time now, LrWpanPhyEnumeration oldState, LrWpanPhyEnumeration newState)
{
  NS_LOG_UNCOND (context << " изменение состояния в " << now.GetSeconds ()
                         << " от " << LrWpanHelper::LrWpanPhyEnumerationPrinter (oldState)
                         << " к " << LrWpanHelper::LrWpanPhyEnumerationPrinter (newState));
}

static void StateChangeNotificationLora (std::string context, Time now, LoRaWANPhyEnumeration oldState, LoRaWANPhyEnumeration newState)
{
  NS_LOG_UNCOND (context << " изменение состояния в " << now.GetSeconds ()
                         << " от " << oldState
                         << " к " << newState);
}
*/
int 
main (int argc, char *argv[])
{
  bool verbose = false;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);
  
  cmd.Parse (argc,argv);

  Ptr<Node> n0 = CreateObject <Node> ();
  //Ptr<LrWpanNetDevice> dev0 = CreateObject<LrWpanNetDevice> ();

  Ptr<Node> n1 = CreateObject <Node> ();
  //Ptr<LoRaWANNetDevice> dev1 = CreateObject<LoRaWANNetDevice> ();

 // dev0->SetAddress (Mac16Address ("00:01"));
  //dev1->SetAddress (Mac16Address ("00:02"));


  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();

  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);

 // dev0->SetChannel (channel);
  //dev1->SetChannel (channel);

 // n0->AddDevice (dev0); // Для завершения настройки необходимо добавить LrWpanNetDevice к узлу
 // n1->AddDevice (dev1); // Для завершения настройки необходимо добавить LoraWanNetDevice к узлу

  //dev0->GetPhy ()->TraceConnect ("TrxState", std::string ("phy0"), MakeCallback (&StateChangeNotificationZigbee));
  //dev1->GetPhy ()->TraceConnect ("TrxState", std::string ("phy1"), MakeCallback (&StateChangeNotificationLora));

  Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender0Mobility->SetPosition (Vector (0,0,0));
 // dev0->GetPhy ()->SetMobility (sender0Mobility);
  Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender1Mobility->SetPosition (Vector (0,10,0));
  //dev1->GetPhy ()->SetMobility (sender1Mobility);
  //McpsDataConfirmCallback cb0;
 // cb0 = MakeCallback (&DataConfirmZigbee);
  //dev0->GetMac ()->SetMcpsDataConfirmCallback (cb0);

  //McpsDataIndicationCallback cb1;
 // cb1 = MakeCallback (&DataIndicationZigbee);
  //dev0->GetMac ()->SetMcpsDataIndicationCallback (cb1);

//  DataConfirmCallback cb2 = MakeCallback (&DataConfirmLora);
 // dev1->GetMac ()->SetDataConfirmCallback (cb2);

//  DataIndicationCallback cb3 = MakeCallback (&DataIndicationLora);
 // dev1->GetMac ()->SetDataIndicationCallback (cb3);

 /* Ptr<Packet> p0 = Create<Packet> (50);  // 50 байт данных
  McpsDataRequestParams params;
  params.m_dstPanId = 0;
  params.m_srcAddrMode = SHORT_ADDR;
  params.m_dstAddrMode = SHORT_ADDR;
  params.m_dstAddr = Mac16Address ("00:02");
  params.m_msduHandle = 0;
  params.m_txOptions = TX_OPTION_ACK;
  dev0->GetMac ()->McpsDataRequest (params, p0);
 // Simulator::ScheduleWithContext (1, Seconds (0.0),
  //                                &LrWpanMac::McpsDataRequest,
 //                                 dev0->GetMac (), params, p0);
  
  Ptr<Packet> p2 = Create<Packet> (60);  // 60 байт данных
  params.m_dstAddr = Mac16Address ("00:01");
  //Simulator::ScheduleWithContext (2, Seconds (2.0),
    //                              &LoRaWANMac::sendMACPayloadRequest,
  //                                dev1->GetMac (), params, p2);
*/
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


