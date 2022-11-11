#include <ns3/log.h>
#include <ns3/core-module.h>
#include <ns3/lora-zigbee-module.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/node.h>
#include <ns3/packet.h>
#include "ns3/rng-seed-manager.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lora-zigbee-retransmit-timeout-test");

class LoRaZiGbeeRetransmitTimeoutTestCase : public TestCase
{
public:
  LoRaZiGbeeRetransmitTimeoutTestCase ();

  static void DataConfirm (LoRaZiGbeeRetransmitTimeoutTestCase *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params);

private:
  virtual void DoRun (void);
  LoRaZiGbeeMcpsDataConfirmStatus confirmParamsStatus;
};

LoRaZiGbeeRetransmitTimeoutTestCase::LoRaZiGbeeRetransmitTimeoutTestCase ()
  : TestCase ("Test the LoRaZiGbee retransmission handling (including RDC)")
{
  confirmParamsStatus = LORAZIGBEE_SUCCESS;
}

void
LoRaZiGbeeRetransmitTimeoutTestCase::DataConfirm (LoRaZiGbeeRetransmitTimeoutTestCase *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params)
{
  testCase->confirmParamsStatus = params.m_status;
}

void
LoRaZiGbeeRetransmitTimeoutTestCase::DoRun (void)
{
  // Set the random seed and run number for this test
  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (6);

  // Create 1 node, and a NetDevice
  Ptr<Node> n0 = CreateObject <Node> ();

  Ptr<LoRaZiGbeeNetDevice> dev0 = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_END_DEVICE_CLASS_A);

  // Make random variable stream assignment deterministic
  //dev0->AssignStreams (0);

  dev0->SetAddress (Ipv4Address (0x00000001));

  // Each device must be attached to the same channel
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);

  dev0->SetChannel (channel);

  // To complete configuration, a LoRaZiGbeeNetDevice must be added to a node
  n0->AddDevice (dev0);

  Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender0Mobility->SetPosition (Vector (0,0,0));
  dev0->GetPhy ()->SetMobility (sender0Mobility);

  DataConfirmCallback cb0;
  cb0 = MakeBoundCallback (&LoRaZiGbeeRetransmitTimeoutTestCase::DataConfirm, this, dev0);
  dev0->GetMac ()->SetDataConfirmCallback (cb0);

  Ptr<Packet> p0 = Create<Packet> (20);  // 20 bytes of dummy data

  LoRaZiGbeeDataRequestParams params;
  params.m_loraZiGbeeChannelIndex = 0;
  params.m_loraZiGbeeDataRateIndex = 5;
  params.m_loraZiGbeeCodeRate = 3;
  params.m_msgType = LORAZIGBEE_CONFIRMED_DATA_UP;
  params.m_requestHandle = 1;
  params.m_numberOfTransmissions = 3;

  Simulator::ScheduleNow (&LoRaZiGbeeMac::sendMACPayloadRequest, dev0->GetMac(), params, p0);

  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ ((confirmParamsStatus == LORAZIGBEE_NO_ACK), true, "ConfirmData status is not set to LORAZIGBEE_NO_ACK. status =  " << confirmParamsStatus);

  Simulator::Destroy ();
}

class LoRaZiGbeeRetransmitTimeoutTestSuite  : public TestSuite
{
public:
  LoRaZiGbeeRetransmitTimeoutTestSuite ();
};

LoRaZiGbeeRetransmitTimeoutTestSuite::LoRaZiGbeeRetransmitTimeoutTestSuite ()
  : TestSuite ("lorazigbee-retransmit-timeout", UNIT)
{
  AddTestCase (new LoRaZiGbeeRetransmitTimeoutTestCase, TestCase::QUICK);
}

static LoRaZiGbeeRetransmitTimeoutTestSuite g_loraZiGbeeAckTimeoutTestSuite;
