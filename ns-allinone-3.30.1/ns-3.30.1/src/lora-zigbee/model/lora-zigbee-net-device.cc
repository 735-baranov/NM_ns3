#include "lora-zigbee-net-device.h"
#include "lora-zigbee-phy.h"
#include "lora-zigbee-csmaca.h"
#include "lora-zigbee-error-model.h"
#include <ns3/abort.h>
#include <ns3/node.h>
#include <ns3/log.h>
#include <ns3/spectrum-channel.h>
#include <ns3/pointer.h>
#include <ns3/boolean.h>
#include <ns3/mobility-model.h>
#include <ns3/packet.h>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LoraZigBeeNetDevice");

NS_OBJECT_ENSURE_REGISTERED (LoraZigBeeNetDevice);

TypeId
LoraZigBeeNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LoraZigBeeNetDevice")
    .SetParent<NetDevice> ()
    .SetGroupName ("LoraZigBee")
    .AddConstructor<LoraZigBeeNetDevice> ()
    .AddAttribute ("Channel", "The channel attached to this device",
                   PointerValue (),
                   MakePointerAccessor (&LoraZigBeeNetDevice::DoGetChannel),
                   MakePointerChecker<SpectrumChannel> ())
    .AddAttribute ("Phy", "The PHY layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&LoraZigBeeNetDevice::GetPhy,
                                        &LoraZigBeeNetDevice::SetPhy),
                   MakePointerChecker<LoraZigBeePhy> ())
    .AddAttribute ("Mac", "The MAC layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&LoraZigBeeNetDevice::GetMac,
                                        &LoraZigBeeNetDevice::SetMac),
                   MakePointerChecker<LoraZigBeeMac> ())
    .AddAttribute ("UseAcks", "Request acknowledgments for data frames.",
                   BooleanValue (true),
                   MakeBooleanAccessor (&LoraZigBeeNetDevice::m_useAcks),
                   MakeBooleanChecker ())
  ;
  return tid;
}

LoraZigBeeNetDevice::LoraZigBeeNetDevice ()
  : m_configComplete (false)
{
  NS_LOG_FUNCTION (this);
  m_mac = CreateObject<LoraZigBeeMac> ();
  m_phy = CreateObject<LoraZigBeePhy> ();
  m_csmaca = CreateObject<LoraZigBeeCsmaCa> ();
  CompleteConfig ();
}

LoraZigBeeNetDevice::~LoraZigBeeNetDevice ()
{
  NS_LOG_FUNCTION (this);
}


void
LoraZigBeeNetDevice::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_mac->Dispose ();
  m_phy->Dispose ();
  m_csmaca->Dispose ();
  m_phy = 0;
  m_mac = 0;
  m_csmaca = 0;
  m_node = 0;
  // chain up.
  NetDevice::DoDispose ();

}

void
LoraZigBeeNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_phy->Initialize ();
  m_mac->Initialize ();
  NetDevice::DoInitialize ();
}


void
LoraZigBeeNetDevice::CompleteConfig (void)
{
  NS_LOG_FUNCTION (this);
  if (m_mac == 0
      || m_phy == 0
      || m_csmaca == 0
      || m_node == 0
      || m_configComplete)
    {
      return;
    }
  m_mac->SetPhy (m_phy);
  m_mac->SetCsmaCa (m_csmaca);
  m_mac->SetMcpsDataIndicationCallback (MakeCallback (&LoraZigBeeNetDevice::McpsDataIndication, this));
  m_csmaca->SetMac (m_mac);

  Ptr<MobilityModel> mobility = m_node->GetObject<MobilityModel> ();
  if (!mobility)
    {
      NS_LOG_WARN ("LoraZigBeeNetDevice: no Mobility found on the node, probably it's not a good idea.");
    }
  m_phy->SetMobility (mobility);
  Ptr<LoraZigBeeErrorModel> model = CreateObject<LoraZigBeeErrorModel> ();
  m_phy->SetErrorModel (model);
  m_phy->SetDevice (this);

  m_phy->SetPdDataIndicationCallback (MakeCallback (&LoraZigBeeMac::PdDataIndication, m_mac));
  m_phy->SetPdDataConfirmCallback (MakeCallback (&LoraZigBeeMac::PdDataConfirm, m_mac));
  m_phy->SetPlmeEdConfirmCallback (MakeCallback (&LoraZigBeeMac::PlmeEdConfirm, m_mac));
  m_phy->SetPlmeGetAttributeConfirmCallback (MakeCallback (&LoraZigBeeMac::PlmeGetAttributeConfirm, m_mac));
  m_phy->SetPlmeSetTRXStateConfirmCallback (MakeCallback (&LoraZigBeeMac::PlmeSetTRXStateConfirm, m_mac));
  m_phy->SetPlmeSetAttributeConfirmCallback (MakeCallback (&LoraZigBeeMac::PlmeSetAttributeConfirm, m_mac));

  m_csmaca->SetLoraZigBeeMacStateCallback (MakeCallback (&LoraZigBeeMac::SetLoraZigBeeMacState, m_mac));
  m_phy->SetPlmeCcaConfirmCallback (MakeCallback (&LoraZigBeeCsmaCa::PlmeCcaConfirm, m_csmaca));
  m_configComplete = true;
}

void
LoraZigBeeNetDevice::SetMac (Ptr<LoraZigBeeMac> mac)
{
  NS_LOG_FUNCTION (this);
  m_mac = mac;
  CompleteConfig ();
}

void
LoraZigBeeNetDevice::SetPhy (Ptr<LoraZigBeePhy> phy)
{
  NS_LOG_FUNCTION (this);
  m_phy = phy;
  CompleteConfig ();
}

void
LoraZigBeeNetDevice::SetCsmaCa (Ptr<LoraZigBeeCsmaCa> csmaca)
{
  NS_LOG_FUNCTION (this);
  m_csmaca = csmaca;
  CompleteConfig ();
}

void
LoraZigBeeNetDevice::SetChannel (Ptr<SpectrumChannel> channel)
{
  NS_LOG_FUNCTION (this << channel);
  m_phy->SetChannel (channel);
  channel->AddRx (m_phy);
  CompleteConfig ();
}

Ptr<LoraZigBeeMac>
LoraZigBeeNetDevice::GetMac (void) const
{
  // NS_LOG_FUNCTION (this);
  return m_mac;
}

Ptr<LoraZigBeePhy>
LoraZigBeeNetDevice::GetPhy (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy;
}

Ptr<LoraZigBeeCsmaCa>
LoraZigBeeNetDevice::GetCsmaCa (void) const
{
  NS_LOG_FUNCTION (this);
  return m_csmaca;
}
void
LoraZigBeeNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  m_ifIndex = index;
}

uint32_t
LoraZigBeeNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ifIndex;
}

Ptr<Channel>
LoraZigBeeNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy->GetChannel ();
}

void
LoraZigBeeNetDevice::LinkUp (void)
{
  NS_LOG_FUNCTION (this);
  m_linkUp = true;
  m_linkChanges ();
}

void
LoraZigBeeNetDevice::LinkDown (void)
{
  NS_LOG_FUNCTION (this);
  m_linkUp = false;
  m_linkChanges ();
}

Ptr<SpectrumChannel>
LoraZigBeeNetDevice::DoGetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy->GetChannel ();
}

void
LoraZigBeeNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this);
  m_mac->SetShortAddress (Mac16Address::ConvertFrom (address));
}

Address
LoraZigBeeNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mac->GetShortAddress ();
}

bool
LoraZigBeeNetDevice::SetMtu (const uint16_t mtu)
{
  NS_ABORT_MSG ("Unsupported");
  return false;
}

uint16_t
LoraZigBeeNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION (this);
  // Maximum payload size is: max psdu - frame control - seqno - addressing - security - fcs
  //                        = 127      - 2             - 1     - (2+2+2+2)  - 0        - 2
  //                        = 114
  // assuming no security and addressing with only 16 bit addresses without pan id compression.
  return 114;
}

bool
LoraZigBeeNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy != 0 && m_linkUp;
}

void
LoraZigBeeNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION (this);
  m_linkChanges.ConnectWithoutContext (callback);
}

bool
LoraZigBeeNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

Address
LoraZigBeeNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return Mac16Address ("ff:ff");
}

bool
LoraZigBeeNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

Address
LoraZigBeeNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_ABORT_MSG ("Unsupported");
  return Address ();
}

Address
LoraZigBeeNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this);
  /* Implementation based on RFC 4944 Section 9.
   * An IPv6 packet with a multicast destination address (DST),
   * consisting of the sixteen octets DST[1] through DST[16], is
   * transmitted to the following 802.15.4 16-bit multicast address:
   *           0                   1
   *           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
   *          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   *          |1 0 0|DST[15]* |   DST[16]     |
   *          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * Here, DST[15]* refers to the last 5 bits in octet DST[15], that is,
   * bits 3-7 within DST[15].  The initial 3-bit pattern of "100" follows
   * the 16-bit address format for multicast addresses (Section 12). */

  // \todo re-add this once Lr-Wpan will be able to accept these multicast addresses
  //  uint8_t buf[16];
  //  uint8_t buf2[2];
  //
  //  addr.GetBytes(buf);
  //
  //  buf2[0] = 0x80 | (buf[14] & 0x1F);
  //  buf2[1] = buf[15];
  //
  //  Mac16Address newaddr = Mac16Address();
  //  newaddr.CopyFrom(buf2);
  //  return newaddr;

  return Mac16Address ("ff:ff");
}

bool
LoraZigBeeNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

bool
LoraZigBeeNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

bool
LoraZigBeeNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  // This method basically assumes an 802.3-compliant device, but a raw
  // 802.15.4 device does not have an ethertype, and requires specific
  // McpsDataRequest parameters.
  // For further study:  how to support these methods somehow, such as
  // inventing a fake ethertype and packet tag for McpsDataRequest
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);

  if (packet->GetSize () > GetMtu ())
    {
      NS_LOG_ERROR ("Fragmentation is needed for this packet, drop the packet ");
      return false;
    }

  McpsDataRequestParams m_mcpsDataRequestParams;
  m_mcpsDataRequestParams.m_dstAddr = Mac16Address::ConvertFrom (dest);
  m_mcpsDataRequestParams.m_dstAddrMode = SHORT_ADDR;
  m_mcpsDataRequestParams.m_dstPanId = m_mac->GetPanId ();
  m_mcpsDataRequestParams.m_srcAddrMode = SHORT_ADDR;
  // Using ACK requests for broadcast destinations is ok here. They are disabled
  // by the MAC.
  if (m_useAcks)
    {
      m_mcpsDataRequestParams.m_txOptions = TX_OPTION_ACK;
    }
  m_mcpsDataRequestParams.m_msduHandle = 0;
  m_mac->McpsDataRequest (m_mcpsDataRequestParams, packet);
  return true;
}

bool
LoraZigBeeNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_ABORT_MSG ("Unsupported");
  // TODO: To support SendFrom, the MACs McpsDataRequest has to use the provided source address, instead of to local one.
  return false;
}

Ptr<Node>
LoraZigBeeNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION (this);
  return m_node;
}

void
LoraZigBeeNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
  CompleteConfig ();
}

bool
LoraZigBeeNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

void
LoraZigBeeNetDevice::SetReceiveCallback (ReceiveCallback cb)
{
  NS_LOG_FUNCTION (this);
  m_receiveCallback = cb;
}

void
LoraZigBeeNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  // This method basically assumes an 802.3-compliant device, but a raw
  // 802.15.4 device does not have an ethertype, and requires specific
  // McpsDataIndication parameters.
  // For further study:  how to support these methods somehow, such as
  // inventing a fake ethertype and packet tag for McpsDataRequest
  NS_LOG_WARN ("Unsupported; use LoraZigBee MAC APIs instead");
}

void
LoraZigBeeNetDevice::McpsDataIndication (McpsDataIndicationParams params, Ptr<Packet> pkt)
{
  NS_LOG_FUNCTION (this);
  // TODO: Use the PromiscReceiveCallback if the MAC is in promiscuous mode.
  m_receiveCallback (this, pkt, 0, params.m_srcAddr);
}

bool
LoraZigBeeNetDevice::SupportsSendFrom (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return false;
}

int64_t
LoraZigBeeNetDevice::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (stream);
  int64_t streamIndex = stream;
  streamIndex += m_csmaca->AssignStreams (stream);
  streamIndex += m_phy->AssignStreams (stream);
  NS_LOG_DEBUG ("Number of assigned RV streams:  " << (streamIndex - stream));
  return (streamIndex - stream);
}

} // namespace ns3
