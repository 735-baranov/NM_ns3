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

NS_LOG_COMPONENT_DEFINE ("lorazigbee-ack-test");

class LoRaZiGbeeAckTestCase : public TestCase
{
public:
  LoRaZiGbeeAckTestCase ();

  static void DataIndication (LoRaZiGbeeAckTestCase *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p);
  static void DataConfirm (LoRaZiGbeeAckTestCase *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params);

private:
  virtual void DoRun (void);
  LoRaZiGbeeMcpsDataConfirmStatus confirmParamsStatus;
  Ipv4Address nodeAddr;
};

LoRaZiGbeeAckTestCase::LoRaZiGbeeAckTestCase ()
  : TestCase ("Test the LoRaZiGbee ACK handling")
{
  confirmParamsStatus = LORAZIGBEE_NO_ACK;
  nodeAddr = Ipv4Address (0x00000001);
}

void
LoRaZiGbeeAckTestCase::DataConfirm (LoRaZiGbeeAckTestCase *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params)
{
  NS_LOG_UNCOND ("DataConfirm( " << dev->GetAddress() << "): status  = " << params.m_status);

  if (dev->GetAddress () == testCase->nodeAddr) {
    testCase->confirmParamsStatus = params.m_status;
  }
}

void
LoRaZiGbeeAckTestCase::DataIndication (LoRaZiGbeeAckTestCase *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("DataIndication ( " << dev->GetAddress() << "): Received packet of size " << p->GetSize ());
}

void
LoRaZiGbeeAckTestCase::DoRun (void)
{
  // Test setup:
  // One class A end device and a gateway
  // Class A ED sends a confirmed data UP and we schedule an a downstream Ack
  // manually in RW1 or RW2 of the class A ED
  bool ackInRW1 = true; // send ack in RW1, setting this to false will send the ack in RW2

  // Set the random seed and run number for this test
  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (6);

  // Create 2 nodes, and a NetDevice for each one
  Ptr<Node> n0 = CreateObject <Node> ();
  Ptr<Node> gw = CreateObject <Node> ();

  Ptr<LoRaZiGbeeNetDevice> dev0 = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_END_DEVICE_CLASS_A);
  Ptr<LoRaZiGbeeNetDevice> dev1 = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_GATEWAY);

  // Make random variable stream assignment deterministic
  //dev0->AssignStreams (0);

  dev0->SetAddress (nodeAddr);
  // dev1->SetAddress (Ipv4Address (0x00000002)); // gateways don't have a network address

  // Each device must be attached to the same channel
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);

  dev0->SetChannel (channel);
  dev1->SetChannel (channel);

  // To complete configuration, a LoraZiGbeeNetDevice must be added to a node
  n0->AddDevice (dev0);
  gw->AddDevice (dev1);

  Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender0Mobility->SetPosition (Vector (0,5,0));
  dev0->GetPhy ()->SetMobility (sender0Mobility);

  Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender1Mobility->SetPosition (Vector (0,0,0));
  for (auto &it : dev1->GetPhys() ) {
    it->SetMobility (sender1Mobility);
  }

  DataConfirmCallback cb0;
  cb0 = MakeBoundCallback (&LoRaZiGbeeAckTestCase::DataConfirm, this, dev0);
  dev0->GetMac ()->SetDataConfirmCallback (cb0);

  DataIndicationCallback cb1;
  cb1 = MakeBoundCallback (&LoRaZiGbeeAckTestCase::DataIndication, this, dev0);
  dev0->GetMac ()->SetDataIndicationCallback (cb1);

  DataConfirmCallback cb2 = MakeBoundCallback (&LoRaZiGbeeAckTestCase::DataConfirm, this, dev1);
  DataIndicationCallback cb3 = MakeBoundCallback (&LoRaZiGbeeAckTestCase::DataIndication, this, dev1);

  for (auto &it : dev1->GetMacs() ) {
    it->SetDataConfirmCallback (cb2);
    it->SetDataIndicationCallback (cb3);
  }

  Ptr<Packet> p0 = Create<Packet> (20);  // 20 bytes of dummy data

  LoRaZiGbeeDataRequestParams params;
  params.m_loraZiGbeeChannelIndex = 0;
  params.m_loraZiGbeeDataRateIndex = 5;
  params.m_loraZiGbeeCodeRate = 3;
  params.m_msgType = LORAZIGBEE_CONFIRMED_DATA_UP;
  params.m_requestHandle = 1;
  params.m_numberOfTransmissions = 3;

  Simulator::ScheduleNow (&LoRaZiGbeeMac::sendMACPayloadRequest, dev0->GetMac (), params, p0);

  // Create Ack packet
  Ptr<Packet> p1 = Create<Packet> (0);  // empty packet
  LoRaZiGbeeFrameHeader frmHdr;
  frmHdr.setDevAddr (nodeAddr);
  frmHdr.setAck (true);
  frmHdr.setFramePending (false);
  frmHdr.setFrameCounter (1);
  frmHdr.setSerializeFramePort (false); // No Frame Port
  p1->AddHeader (frmHdr);

  params.m_loraZiGbeeChannelIndex = 0;
  params.m_loraZiGbeeDataRateIndex = 5;
  params.m_loraZiGbeeCodeRate = 3;
  params.m_msgType = LORAZIGBEE_UNCONFIRMED_DATA_DOWN;
  params.m_requestHandle = 2;
  params.m_numberOfTransmissions = 1;

  // Node will open RW1 after end of TX + 1 second. 20B of payload at SF7/CR3
  // takes ~78ms to send, so schedule Ack at 1s + 80ms
  Time delay = Seconds(1.080);
  if (!ackInRW1)
    delay = Seconds (3.095);

  // Find correct MAC object to send DS Ack:
  uint8_t macIndex = 0;
  if (dev1->getMACSIndexForChannelAndDataRate (macIndex, params.m_loraZiGbeeChannelIndex, params.m_loraZiGbeeDataRateIndex)) {
    if (macIndex >= 0 && macIndex < dev1->GetMacs ().size ()) {
      Ptr<LoRaZiGbeeMac> macPtr = dev1->GetMacs ()[macIndex];
      Simulator::Schedule (delay, &LoRaZiGbeeMac::sendMACPayloadRequest, macPtr, params, p1);
    } else {
      NS_ASSERT_MSG (false, " Unable to find corresponding MAC object on GW for sending DS transmission");
    }
  } else {
    NS_ASSERT_MSG (false, " Unable to find corresponding MAC object on GW for sending DS transmission");
  }


  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ ((confirmParamsStatus == LORAZIGBEE_SUCCESS), true, "ConfirmData status is not set to LORAZIGBEE_SUCCESS. status = " << confirmParamsStatus);

  Simulator::Destroy ();
}

class LoRaZiGbeeAckTestSuite  : public TestSuite
{
public:
  LoRaZiGbeeAckTestSuite ();
};

LoRaZiGbeeAckTestSuite::LoRaZiGbeeAckTestSuite ()
  : TestSuite ("lorazigbee-ack", UNIT)
{
  AddTestCase (new LoRaZiGbeeAckTestCase, TestCase::QUICK);
}

static LoRaZiGbeeAckTestSuite g_loraZiGbeeAckTestSuite;
