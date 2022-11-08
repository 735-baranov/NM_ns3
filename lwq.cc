#include <ns3/log.h>
#include <ns3/core-module.h>
#include <ns3/lr-wpan-mac.h>
#include <ns3/lr-wpan-net-device.h>
#include <ns3/lr-wpan-helper.h>
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
#include <ns3/wifi-net-device.h>
#include <ns3/wifi-mac.h>
#include <iostream>

using namespace ns3;

static void DataIndication (McpsDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("Индикация данных: полученный пакет для размером  " << p->GetSize ());
}

static void DataConfirm (McpsDataConfirmParams params)
{
  NS_LOG_UNCOND ("Статус подтверждения данных Mcps = " << params.m_status);
}

static void StateChangeNotification (std::string context, Time now, LrWpanPhyEnumeration oldState, LrWpanPhyEnumeration newState)
{
  NS_LOG_UNCOND (context << " изменение состояния в " << now.GetSeconds ()
                         << " от " << LrWpanHelper::LrWpanPhyEnumerationPrinter (oldState)
                         << " к " << LrWpanHelper::LrWpanPhyEnumerationPrinter (newState));
}


int 
main (int argc, char *argv[])
{
  bool verbose = false;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);
  LrWpanHelper lrWpanHelper;
  if (verbose)
    {
      lrWpanHelper.EnableLogComponents ();
    }
/*
  cmd.Parse (argc,argv);

  Ptr<Node> n0 = CreateObject <Node> ();
  Ptr<LrWpanNetDevice> dev0 = CreateObject<LrWpanNetDevice> ();

  Ptr<Node> n1 = CreateObject <Node> ();
  Ptr<WifiNetDevice> dev1 = CreateObject<WifiNetDevice> ();

  dev0->SetAddress (Mac16Address ("00:01"));
  dev1->SetAddress (Mac16Address ("00:02"));


  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();

  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);

  dev0->SetChannel (channel);
  dev1->SetChannel (channel);

  n0->AddDevice (dev0); // Для завершения настройки необходимо добавить LrWpanNetDevice к узлу
  n1->AddDevice (dev1); 

  dev0->GetPhy ()->TraceConnect ("TrxState", std::string ("phy0"), MakeCallback (&StateChangeNotification));
  dev1->GetPhy ()->TraceConnect ("TrxState", std::string ("phy1"), MakeCallback (&StateChangeNotification));

  Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender0Mobility->SetPosition (Vector (0,0,0));
  dev0->GetPhy ()->SetMobility (sender0Mobility);
  Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender1Mobility->SetPosition (Vector (0,10,0));
  dev1->GetPhy ()->SetMobility (sender1Mobility);
  
  McpsDataConfirmCallback cb0;
  cb0 = MakeCallback (&DataConfirm);
  dev0->GetMac ()->SetMcpsDataConfirmCallback (cb0);

  McpsDataIndicationCallback cb1;
  cb1 = MakeCallback (&DataIndication);
  dev0->GetMac ()->SetMcpsDataIndicationCallback (cb1);
 
  McpsDataConfirmCallback cb2 
  cb2 = MakeCallback (&DataConfirm);
  dev1->GetMac ()->SetMcpsDataConfirmCallback (cb2);

  McpsDataIndicationCallback cb3 
  cb3 = MakeCallback (&DataIndication);
  dev1->GetMac ()->SetMcpsDataIndicationCallback (cb3);

  Ptr<Packet> p0 = Create<Packet> (50);  // 50 байт данных
  McpsDataRequestParams params;
  params.m_dstPanId = 0;
  params.m_srcAddrMode = SHORT_ADDR;
  params.m_dstAddrMode = SHORT_ADDR;
  params.m_dstAddr = Mac16Address ("00:02");
  params.m_msduHandle = 0;
  params.m_txOptions = TX_OPTION_ACK;
  dev0->GetMac ()->McpsDataRequest (params, p0);
  Simulator::ScheduleWithContext (1, Seconds (0.0),
                                  &LrWpanMac::McpsDataRequest,
                                  dev0->GetMac (), params, p0);
  
  Ptr<Packet> p2 = Create<Packet> (60);  // 60 байт данных
  params.m_dstAddr = Mac16Address ("00:01");
  Simulator::ScheduleWithContext (2, Seconds (2.0),
                         &WifiMac::McpsDataRequest,
                           dev1->GetMac (), params, p2);

*/
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

