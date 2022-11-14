#include <ns3/log.h>
#include <ns3/core-module.h>
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
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lora-zigbee-example");

static void DataIndicationZigbee (McpsDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("Индикация данных: полученный пакет для Zigbee размером  " << p->GetSize ());
}
static void DataConfirmZigbee (McpsDataConfirmParams params)
{
  NS_LOG_UNCOND ("Статус подтверждения данных ZigbeeMcps = " << params.m_status);
}

static void DataIndicationLora (McpsDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("Индикация данных: полученный пакет для Lora размером  " << p->GetSize ());
}
static void DataConfirmLora(McpsDataConfirmParams params)
{
  NS_LOG_UNCOND ("Статус подтверждения данных LoraMcps = " << params.m_status);
}

static void StateChangeNotification (std::string context, Time now, LoraZigBeePhyEnumeration oldState, LoraZigBeePhyEnumeration newState)
{
  NS_LOG_UNCOND (context << " изменение состояния в " << now.GetSeconds ()
                         << " от " << LoraZigBeeHelper::LoraZigBeePhyEnumerationPrinter (oldState)
                         << " к " << LoraZigBeeHelper::LoraZigBeePhyEnumerationPrinter (newState));
}

int
main (int argc, char *argv[])
{
  bool verbose = false;
  CommandLine cmd;
  cmd.AddValue ("verbose", "turn on all log components", verbose);
  cmd.Parse (argc, argv);
  LoraZigBeeHelper lorazigBeeHelper;
  if (verbose)
    {
     lorazigBeeHelper.EnableLogComponents ();
    }
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
  // Создаем 2 узла и NetDevice для каждого
  Ptr<Node> n0 = CreateObject <Node> ();
  Ptr<LoraZigBeeNetDevice> dev0 = CreateObject<LoraZigBeeNetDevice> ();

  Ptr<Node> n1 = CreateObject <Node> ();
  Ptr<LoraZigBeeNetDevice> dev1 = CreateObject<LoraZigBeeNetDevice> ();
 
  dev0->SetAddress (Mac16Address ("00:01"));
  dev1->SetAddress (Mac16Address ("00:02"));

  // Каждое устройство должно быть подключено к одному и тому же каналу  
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);

  dev0->SetChannel (channel);
  dev1->SetChannel (channel);

  n0->AddDevice (dev0); // Для завершения настройки необходимо добавить LrWpanNetDevice к узлу
  n1->AddDevice (dev1); // Для завершения настройки необходимо добавить LoraWanNetDevice к узлу

  // Отслеживание изменений состояния в phy
  dev0->GetPhy ()->TraceConnect ("TrxState", std::string ("phy0"), MakeCallback (&StateChangeNotification));
  dev1->GetPhy ()->TraceConnect ("TrxState", std::string ("phy1"), MakeCallback (&StateChangeNotification));

  Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender0Mobility->SetPosition (Vector (0,0,0));
  dev0->GetPhy ()->SetMobility (sender0Mobility);
  //Настройка позиции на расстоянии 10 м
  Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender1Mobility->SetPosition (Vector (0,10,0));
  dev1->GetPhy ()->SetMobility (sender1Mobility);

  McpsDataConfirmCallback cb0;
  cb0 = MakeCallback (&DataConfirmZigbee);
  dev0->GetMac ()->SetMcpsDataConfirmCallback (cb0);

  McpsDataIndicationCallback cb1;
  cb1 = MakeCallback (&DataIndicationZigbee);
  dev0->GetMac ()->SetMcpsDataIndicationCallback (cb1);

  McpsDataConfirmCallback cb2; 
  cb2 = MakeCallback (&DataConfirmLora);
  dev1->GetMac ()->SetMcpsDataConfirmCallback (cb2);

  McpsDataIndicationCallback cb3;
  cb3 = MakeCallback (&DataIndicationLora);
  dev1->GetMac ()->SetMcpsDataIndicationCallback (cb3);

  lorazigBeeHelper.EnablePcapAll (std::string ("lora-zigbee-data"), true);
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("lora-zigbee-data.xml");
  lorazigBeeHelper.EnableAsciiAll (stream);

  // Приведенное ниже должно вызвать два обратных вызова, когда сквозные данные работают
  // 1) Вызывается обратный вызов DataConfirm
  // 2) Обратный вызов DataIndication вызывается со значением 50
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
                                  &LoraZigBeeMac::McpsDataRequest,
                                  dev0->GetMac (), params, p0);
  
  Ptr<Packet> p2 = Create<Packet> (60);  // 60 байт данных
  McpsDataRequestParams params1;
  params1.m_dstPanId = 0;
  params1.m_srcAddrMode = SHORT_ADDR;
  params1.m_dstAddrMode = SHORT_ADDR;
  params1.m_dstAddr = Mac16Address ("00:01");
  params1.m_msduHandle = 1;
  params1.m_txOptions = TX_OPTION_ACK;
  dev1->GetMac ()->McpsDataRequest (params1, p2);
  Simulator::ScheduleWithContext (2, Seconds (2.0),
                                  &LoraZigBeeMac::McpsDataRequest,
                                  dev1->GetMac (), params1, p2);

  Simulator::Run ();

  Simulator::Destroy ();

  return 0;
}