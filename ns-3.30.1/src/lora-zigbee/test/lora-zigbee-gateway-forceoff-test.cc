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

NS_LOG_COMPONENT_DEFINE ("lorazigbee-gateway-forceoff-test");

class LoRaZiGbeeGatewayForceOffTest : public TestCase
{
public:
  LoRaZiGbeeGatewayForceOffTest ();

  static void DataIndication (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p);
  static void DataConfirm (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params);

  static void DataIndicationGateway (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p);
  static void DataConfirmGateway (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params);
private:
  virtual void DoRun (void);

  Ipv4Address node0Addr;
  Ipv4Address node1Addr;

  bool node0USReceived;
  bool node1USReceived;
  bool node0DSReceived;
};

LoRaZiGbeeGatewayForceOffTest::LoRaZiGbeeGatewayForceOffTest ()
  : TestCase ("Test whether a gateway can terminate an ongoing RX of an US TX to initiate a DS TX in RW1 of an end device")
{
  node0Addr = Ipv4Address (0x00000001);
  node1Addr = Ipv4Address (0x00000002);

  node0USReceived = false;
  node1USReceived = false;
  node0DSReceived = false;
}

void
LoRaZiGbeeGatewayForceOffTest::DataConfirm (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params)
{
  NS_LOG_UNCOND ("DataConfirm( " << dev->GetAddress() << "): status  = " << params.m_status);
}

void
LoRaZiGbeeGatewayForceOffTest::DataIndication (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("DataIndication ( " << dev->GetAddress() << "): Received packet of size " << p->GetSize () << " from gateway");

  if (dev->GetAddress () == testCase->node0Addr) {
    testCase->node0DSReceived = true;
  }
}

void
LoRaZiGbeeGatewayForceOffTest::DataConfirmGateway (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataConfirmParams params)
{
  NS_LOG_UNCOND ("DataConfirmGateway : status  = " << params.m_status);
}

void
LoRaZiGbeeGatewayForceOffTest::DataIndicationGateway (LoRaZiGbeeGatewayForceOffTest *testCase, Ptr<LoRaZiGbeeNetDevice> dev, LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p)
{
  NS_LOG_UNCOND ("DataIndicationGateway : Received packet of size " << p->GetSize () << " from end device with address " << dev->GetAddress ());

  // Decode Frame header
  LoRaZiGbeeFrameHeader frmHdr;
  frmHdr.setSerializeFramePort (true); // Assume that frame Header contains Frame Port so set this to true so that RemoveHeader will deserialize the FPort
  p->RemoveHeader (frmHdr);
  // Find end device meta data:
  Ipv4Address deviceAddr = frmHdr.getDevAddr ();

  if (deviceAddr == testCase->node0Addr) {
    testCase->node0USReceived = true;
  } else if (deviceAddr == testCase->node1Addr) {
    testCase->node1USReceived = true;
  }
}

void
LoRaZiGbeeGatewayForceOffTest::DoRun (void)
{
  // Test setup: Two class A end devices and a gateway
  // Device 0 sends US message
  // Device 1 starts sending US message after Device 0 finishes TX but before RW1 of device 0 starts, the device1 transmission is long enough so that it ends during RW1
  // Gateway sends DS message to device 1 in RW1
  // Test whether reception of device 1's transmission is aborted and whether device 1 received the DS transmission

  // Set the random seed and run number for this test
  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (6);

  // Create 2 nodes, and a NetDevice for each one
  Ptr<Node> n0 = CreateObject <Node> ();
  Ptr<Node> n1 = CreateObject <Node> ();
  Ptr<Node> gw = CreateObject <Node> ();

  Ptr<LoRaZiGbeeNetDevice> dev0 = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_END_DEVICE_CLASS_A);
  Ptr<LoRaZiGbeeNetDevice> dev1 = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_END_DEVICE_CLASS_A);
  Ptr<LoRaZiGbeeNetDevice> dev_gw = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_GATEWAY);

  // Make random variable stream assignment deterministic
  //dev0->AssignStreams (0);

  dev0->SetAddress (node0Addr);
  dev1->SetAddress (node1Addr);
  // dev1->SetAddress (Ipv4Address (0x00000002)); // gateways don't have a network address

  // Each device must be attached to the same channel
  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
  Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  channel->AddPropagationLossModel (propModel);
  channel->SetPropagationDelayModel (delayModel);

  dev0->SetChannel (channel);
  dev1->SetChannel (channel);
  dev_gw->SetChannel (channel);

  // To complete configuration, a LoRaZiGbeeNetDevice must be added to a node
  n0->AddDevice (dev0);
  n1->AddDevice (dev1);
  gw->AddDevice (dev_gw);

  Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender0Mobility->SetPosition (Vector (0,5,0));
  dev0->GetPhy ()->SetMobility (sender0Mobility);

  Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
  sender1Mobility->SetPosition (Vector (5,0,0));
  dev1->GetPhy ()->SetMobility (sender1Mobility);

  Ptr<ConstantPositionMobilityModel> gwMobility = CreateObject<ConstantPositionMobilityModel> ();
  gwMobility->SetPosition (Vector (0,0,0));
  for (auto &it : dev_gw->GetPhys() ) {
    it->SetMobility (gwMobility);
  }

  DataConfirmCallback cb0;
  cb0 = MakeBoundCallback (&LoRaZiGbeeGatewayForceOffTest::DataConfirm, this, dev0);
  dev0->GetMac ()->SetDataConfirmCallback (cb0);

  DataIndicationCallback cb1;
  cb1 = MakeBoundCallback (&LoRaZiGbeeGatewayForceOffTest::DataIndication, this, dev0);
  dev0->GetMac ()->SetDataIndicationCallback (cb1);

  DataConfirmCallback cb2;
  cb2 = MakeBoundCallback (&LoRaZiGbeeGatewayForceOffTest::DataConfirm, this, dev1);
  dev1->GetMac ()->SetDataConfirmCallback (cb2);

  DataIndicationCallback cb3;
  cb3 = MakeBoundCallback (&LoRaZiGbeeGatewayForceOffTest::DataIndication, this, dev1);
  dev1->GetMac ()->SetDataIndicationCallback (cb3);

  DataConfirmCallback cb4 = MakeBoundCallback (&LoRaZiGbeeGatewayForceOffTest::DataConfirmGateway, this, dev_gw);
  DataIndicationCallback cb5 = MakeBoundCallback (&LoRaZiGbeeGatewayForceOffTest::DataIndicationGateway, this, dev_gw);

  for (auto &it : dev_gw->GetMacs() ) {
    it->SetDataConfirmCallback (cb4);
    it->SetDataIndicationCallback (cb5);
  }

  // US packet from node 0
  Ptr<Packet> p0 = Create<Packet> (20);  // 20 bytes of dummy data
  LoRaZiGbeeFrameHeader frmHdr0;
  frmHdr0.setDevAddr (node0Addr);
  frmHdr0.setAck (false);
  frmHdr0.setFramePending (false);
  frmHdr0.setFrameCounter (1);
  frmHdr0.setSerializeFramePort (true); // No Frame Port
  p0->AddHeader (frmHdr0);

  LoRaZiGbeeDataRequestParams params;
  params.m_loraZiGbeeChannelIndex = 0;
  params.m_loraZiGbeeDataRateIndex = 5;
  params.m_loraZiGbeeCodeRate = 3;
  params.m_msgType = LORAZIGBEE_UNCONFIRMED_DATA_UP;
  params.m_requestHandle = 1;
  params.m_numberOfTransmissions = 1;
  Simulator::ScheduleNow (&LoRaZiGbeeMac::sendMACPayloadRequest, dev0->GetMac (), params, p0);

  // US packet from node 1, on a different channel -> this forces the gateway to use a different MAC Object for reception
  Ptr<Packet> p1 = Create<Packet> (20);  // 20 bytes of dummy data
  params.m_loraZiGbeeChannelIndex = 0;
  params.m_loraZiGbeeDataRateIndex = 5;
  params.m_loraZiGbeeCodeRate = 3;
  params.m_msgType = LORAZIGBEE_UNCONFIRMED_DATA_UP;
  params.m_requestHandle = 2;
  params.m_numberOfTransmissions = 1;

  Time delay = Seconds(1.100 - 0.040); // 40 ms before the RW1 of node 0, TX of 20B of payload at SF7/CR3 takes ~92ms to send
  // Time delay = Seconds(2.039 + 1.0 - 0.050); // 40 ms before the RW1 of node 0, TX of 20B of payload at SF12/CR3 takes ~2.039s to send
  Simulator::Schedule (delay, &LoRaZiGbeeMac::sendMACPayloadRequest, dev1->GetMac (), params, p1);

  // DS packet from gateway to node 0, should abort reception of p1 by gateway
  Ptr<Packet> p2 = Create<Packet> (20);  // 20 bytes of dummy data
  LoRaZiGbeeFrameHeader frmHdr2;
  frmHdr2.setDevAddr (node0Addr);
  frmHdr2.setAck (false);
  frmHdr2.setFramePending (false);
  frmHdr2.setFrameCounter (1);
  frmHdr2.setSerializeFramePort (true); // No Frame Port
  p2->AddHeader (frmHdr2);

  params.m_loraZiGbeeChannelIndex = 0;
  params.m_loraZiGbeeDataRateIndex = 5;
  params.m_loraZiGbeeCodeRate = 3;
  params.m_msgType = LORAZIGBEE_UNCONFIRMED_DATA_DOWN;
  params.m_requestHandle = 3;
  params.m_numberOfTransmissions = 1;

  // Find correct MAC object to send DS packet:
  uint8_t macIndex = 0;
  if (dev_gw->getMACSIndexForChannelAndDataRate (macIndex, params.m_loraZiGbeeChannelIndex, params.m_loraZiGbeeDataRateIndex)) {
    if (macIndex >= 0 && macIndex < dev_gw->GetMacs ().size ()) {
      Ptr<LoRaZiGbeeMac> macPtr = dev_gw->GetMacs ()[macIndex];

      delay = Seconds(1.100); // Node will open RW1 after end of TX + 1 second. 20B of payload at SF7/CR3 takes ~92ms to send, so schedule DS at 1s + 80ms
      // delay = Seconds (2.039 + 1.0 + 0.1); // SF12/CR3
      Simulator::Schedule (delay, &LoRaZiGbeeMac::sendMACPayloadRequest, macPtr, params, p2);
    } else {
      NS_ASSERT_MSG (false, " Unable to find corresponding MAC object on GW for sending DS transmission");
    }
  } else {
    NS_ASSERT_MSG (false, " Unable to find corresponding MAC object on GW for sending DS transmission");
  }

  Simulator::Stop (Seconds (10.11));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (node0USReceived, true, "Failed to receive US packet from node 0 at GW");
  NS_TEST_ASSERT_MSG_EQ (node1USReceived, false, "Did receive US packet from node 1 at GW, should not happen as PHY should be OFF for DS TX during node 1 transmission");
  NS_TEST_ASSERT_MSG_EQ (node0DSReceived, true, "Failed to receive DS packet from GW at node 0.");

  Simulator::Destroy ();
}

class LoRaZiGbeeGatewayForceOffTestSuite : public TestSuite
{
public:
  LoRaZiGbeeGatewayForceOffTestSuite ();
};

LoRaZiGbeeGatewayForceOffTestSuite::LoRaZiGbeeGatewayForceOffTestSuite ()
  : TestSuite ("lorazigbee-gateway-forceoff", UNIT)
{
  AddTestCase (new LoRaZiGbeeGatewayForceOffTest, TestCase::QUICK);
}

static LoRaZiGbeeGatewayForceOffTestSuite g_loraZiGbeeForceOffTestSuite;
