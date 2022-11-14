#include <ns3/test.h>
#include <ns3/log.h>
#include <ns3/core-module.h>
#include <ns3/callback.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/node.h>
#include <ns3/net-device.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/constant-position-mobility-model.h>
#include "ns3/rng-seed-manager.h"

#include "ns3/lora-zigbee.h"
#include "ns3/lora-zigbee-error-model.h"
#include <ns3/lora-zigbee-module.h>
#include <ns3/lora-zigbee-error-model.h>

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lorazigbee-error-model-test");

class LoRaZiGbeeErrorDistanceTestCase : public TestCase
{
public:
  LoRaZiGbeeErrorDistanceTestCase ();
  virtual ~LoRaZiGbeeErrorDistanceTestCase ();
  uint32_t GetReceived (void) const
  {
    return m_received;
  }

private:
  virtual void DoRun (void);
  void IndicationCallback (LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p);
  uint32_t m_received;
};

LoRaZiGbeeErrorDistanceTestCase::LoRaZiGbeeErrorDistanceTestCase ()
  : TestCase ("Test the lora error model vs distance"),
    m_received (0)
{
}

LoRaZiGbeeErrorDistanceTestCase::~LoRaZiGbeeErrorDistanceTestCase ()
{
}

void
LoRaZiGbeeErrorDistanceTestCase::IndicationCallback (LoRaZiGbeeDataIndicationParams params, Ptr<Packet> p)
{
  m_received++;
}

void
LoRaZiGbeeErrorDistanceTestCase::DoRun (void)
{
  // Set the random seed and run number for this test
  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (6);

  Ptr<Node> n0 = CreateObject <Node> ();
  Ptr<Node> n1 = CreateObject <Node> ();
  Ptr<LoRaZiGbeeNetDevice> dev0 = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_END_DEVICE_CLASS_A);
  Ptr<LoRaZiGbeeNetDevice> devgw = CreateObject<LoRaZiGbeeNetDevice> (LORAZIGBEE_DT_GATEWAY);

  // Make random variable stream assignment deterministic
  dev0->AssignStreams (0);
  devgw->AssignStreams (10);

  Ipv4Address nodeAddr (0x00000001);
  dev0->SetAddress (nodeAddr);

  Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
  Ptr<LogDistancePropagationLossModel> model = CreateObject<LogDistancePropagationLossModel> ();
  channel->AddPropagationLossModel (model);

  dev0->SetChannel (channel);
  devgw->SetChannel (channel);

  // To complete configuration, a LoraZiGbeeNetDevice must be added to a node
  n0->AddDevice (dev0);
  n1->AddDevice (devgw);

  Ptr<ConstantPositionMobilityModel> mob0 = CreateObject<ConstantPositionMobilityModel> ();
  dev0->GetPhy ()->SetMobility (mob0);
  Ptr<ConstantPositionMobilityModel> mob1 = CreateObject<ConstantPositionMobilityModel> ();
  //sender1Mobility->SetPosition (Vector (0,0,0));
  for (auto &it : devgw->GetPhys() ) {
    it->SetMobility (mob1);
  }

  DataIndicationCallback cb0;
  cb0 = MakeCallback (&LoRaZiGbeeErrorDistanceTestCase::IndicationCallback, this);
  for (auto &it : devgw->GetMacs() ) {
    it->SetDataIndicationCallback (cb0);
  }

  LoRaZiGbeeDataRequestParams params;
  params.m_loraZiGbeeChannelIndex = 0;
  params.m_loraZiGbeeDataRateIndex = 5;
  params.m_loraZiGbeeCodeRate = 3;
  params.m_msgType = LORAZIGBEE_UNCONFIRMED_DATA_UP;
  params.m_requestHandle = 1;
  params.m_numberOfTransmissions = 1;

  Ptr<Packet> p;
  mob0->SetPosition (Vector (0,0,0));
  mob1->SetPosition (Vector (2000,0,0));
  for (int i = 0; i < 1000; i++)
    {
      p = Create<Packet> (20);
      Simulator::Schedule(Seconds (4*i),
          &LoRaZiGbeeMac::sendMACPayloadRequest,
          dev0->GetMac (), params, p);
    }


  Simulator::Run ();

  NS_LOG_UNCOND ("Received " << GetReceived () << " packets");
  // Test that we received 977 packets out of 1000, at distance of 100 m
  // with default power of 0
  NS_TEST_ASSERT_MSG_EQ (GetReceived (), 954, "Model fails");

  Simulator::Destroy ();
}

// ==============================================================================
class LoRaZiGbeeErrorModelTestCase : public TestCase
{
public:
  LoRaZiGbeeErrorModelTestCase ();
  virtual ~LoRaZiGbeeErrorModelTestCase ();

private:
  virtual void DoRun (void);
};

LoRaZiGbeeErrorModelTestCase::LoRaZiGbeeErrorModelTestCase ()
  : TestCase ("Test the LoRaZiGbee error model")
{
}

LoRaZiGbeeErrorModelTestCase::~LoRaZiGbeeErrorModelTestCase ()
{
}

void
LoRaZiGbeeErrorModelTestCase::DoRun (void)
{
  //std::cout << "Starting BER calculation tests " << std::endl;

  Ptr<LoRaZiGbeeErrorModel> model = CreateObject<LoRaZiGbeeErrorModel> ();
  uint32_t bandwidth = 125e3;

  // TODO: extend test cases

  // SF7, CR3
  LoRaSpreadingFactor spreadingFactor = static_cast <LoRaSpreadingFactor> (7);
  uint8_t codeRate = 3;

  double snr = -5.0;
  double ber = model->getBER (snr, bandwidth, spreadingFactor, codeRate);
  ber = model->getBER (snr, bandwidth, spreadingFactor, codeRate);
  NS_TEST_ASSERT_MSG_EQ_TOL (ber, 6.7884e-17, 1.0e-19, "Error Model fails for SNR = " << snr << " BER = " << ber);

  snr = -8.0;
  ber = model->getBER (snr, bandwidth, spreadingFactor, codeRate);
  NS_TEST_ASSERT_MSG_EQ_TOL (ber, 5.54569e-06, 0.00001, "Error Model fails for SNR = " << snr << " BER = " << ber);

  snr = -10.0;
  ber = model->getBER (snr, bandwidth, spreadingFactor, codeRate);
  NS_TEST_ASSERT_MSG_EQ_TOL (ber, 0.00327354, 0.00001, "Error Model fails for SNR = " << snr << " BER = " << ber);

}

// ==============================================================================
class LoRaZiGbeeErrorModelTestSuite : public TestSuite
{
public:
  LoRaZiGbeeErrorModelTestSuite ();
};

LoRaZiGbeeErrorModelTestSuite::LoRaZiGbeeErrorModelTestSuite ()
  : TestSuite ("lorazigbee-error-model", UNIT)
{
  AddTestCase (new LoRaZiGbeeErrorModelTestCase, TestCase::QUICK);
  AddTestCase (new LoRaZiGbeeErrorDistanceTestCase, TestCase::QUICK);
}

static LoRaZiGbeeErrorModelTestSuite lorazigbeeErrorModelTestSuite;
