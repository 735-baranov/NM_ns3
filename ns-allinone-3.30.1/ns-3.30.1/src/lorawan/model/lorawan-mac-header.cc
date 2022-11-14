#include "lorawan-mac-header.h"
#include "lorawan-mac.h"
#include <ns3/address-utils.h>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LoRaWANMacHeader);

LoRaWANMacHeader::LoRaWANMacHeader ()
{
  setLoRaWANMsgType (LoRaWANMsgType::LORAWAN_UNCONFIRMED_DATA_UP);
  setMajor (0);
}

LoRaWANMacHeader::LoRaWANMacHeader (LoRaWANMsgType mType,
                                  uint8_t major)
{
  setLoRaWANMsgType (mType);
  setMajor (major);
}

LoRaWANMacHeader::~LoRaWANMacHeader ()
{
}

LoRaWANMsgType
LoRaWANMacHeader::getLoRaWANMsgType (void) const
{
  return m_msgType;
}

void
LoRaWANMacHeader::setLoRaWANMsgType (LoRaWANMsgType type)
{
  m_msgType = type;
}

uint8_t
LoRaWANMacHeader::getMajor (void) const
{
  return m_major;
}

void
LoRaWANMacHeader::setMajor (uint8_t major)
{
  m_major = major;
}

TypeId
LoRaWANMacHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LoRaWANMacHeader")
    .SetParent<Header> ()
    .SetGroupName ("LoRaWAN")
    .AddConstructor<LoRaWANMacHeader> ();
  return tid;
}

TypeId
LoRaWANMacHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LoRaWANMacHeader::Print (std::ostream &os) const
{
  os << "  Message Type = " << (uint32_t) m_msgType << ", Major = " << (uint32_t) m_major;
}

uint32_t
LoRaWANMacHeader::GetSerializedSize (void) const
{
  /*
   * Each mac header will have 1 octet
   */

  return 1;
}

void
LoRaWANMacHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  uint8_t mhdr = 0; // mhdr: msg type (3 bits), RFU (3 bits) and Major version (2 bits)
  mhdr |= (m_msgType & 0x07) << 5;
  mhdr |= (m_major & 0x3);

  i.WriteU8 (mhdr);
}

uint32_t
LoRaWANMacHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint8_t mhdr = i.ReadU8();
  LoRaWANMsgType msgType = static_cast<LoRaWANMsgType>((mhdr >> 5) & 0x07);

  setLoRaWANMsgType (msgType);
  setMajor ((uint8_t)mhdr & 0x03);

  return 1;
}

bool
LoRaWANMacHeader::IsConfirmed () const
{
  return (m_msgType == LORAWAN_CONFIRMED_DATA_UP || m_msgType == LORAWAN_CONFIRMED_DATA_DOWN);
}

bool
LoRaWANMacHeader::IsDownstream() const
{
  return (m_msgType == LORAWAN_UNCONFIRMED_DATA_DOWN || m_msgType == LORAWAN_CONFIRMED_DATA_DOWN);
}

bool
LoRaWANMacHeader::IsUpstream() const
{
  return (m_msgType == LORAWAN_CONFIRMED_DATA_UP || m_msgType == LORAWAN_UNCONFIRMED_DATA_UP);
}

// ----------------------------------------------------------------------------------------------------------

} //namespace ns3
