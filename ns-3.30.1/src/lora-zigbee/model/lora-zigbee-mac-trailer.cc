#include "lora-zigbee-mac-trailer.h"
#include <ns3/packet.h>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LoraZigBeeMacTrailer);

const uint16_t LoraZigBeeMacTrailer::LORA_ZIGBEE_MAC_FCS_LENGTH = 2;

LoraZigBeeMacTrailer::LoraZigBeeMacTrailer (void)
  : m_fcs (0),
    m_calcFcs (false)
{
}

TypeId
LoraZigBeeMacTrailer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LoraZigBeeMacTrailer")
    .SetParent<Trailer> ()
    .SetGroupName ("LoraZigBee")
    .AddConstructor<LoraZigBeeMacTrailer> ()
  ;
  return tid;
}

TypeId
LoraZigBeeMacTrailer::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
LoraZigBeeMacTrailer::Print (std::ostream &os) const
{
  os << " FCS = " << m_fcs;
}

uint32_t
LoraZigBeeMacTrailer::GetSerializedSize (void) const
{
  return LORA_ZIGBEE_MAC_FCS_LENGTH;
}

void
LoraZigBeeMacTrailer::Serialize (Buffer::Iterator start) const
{
  start.Prev (LORA_ZIGBEE_MAC_FCS_LENGTH);
  start.WriteU16 (m_fcs);
}

uint32_t
LoraZigBeeMacTrailer::Deserialize (Buffer::Iterator start)
{
  start.Prev (LORA_ZIGBEE_MAC_FCS_LENGTH);
  m_fcs = start.ReadU16 ();

  return LORA_ZIGBEE_MAC_FCS_LENGTH;
}

uint16_t
LoraZigBeeMacTrailer::GetFcs (void) const
{
  return m_fcs;
}

void
LoraZigBeeMacTrailer::SetFcs (Ptr<const Packet> p)
{
  if (m_calcFcs)
    {
      uint16_t size = p->GetSize ();
      uint8_t *serial_packet = new uint8_t[size];

      p->CopyData (serial_packet, size);

      m_fcs = GenerateCrc16 (serial_packet, size);
      delete[] serial_packet;
    }
}

/* Be sure to have removed the trailer and only the trailer
 * from the packet before to use CheckFcs */
bool
LoraZigBeeMacTrailer::CheckFcs (Ptr<const Packet> p)
{
  if (!m_calcFcs)
    {
      return true;
    }
  else
    {
      uint16_t checkFcs;
      uint16_t size = p->GetSize ();
      uint8_t *serial_packet = new uint8_t[size];

      p->CopyData (serial_packet, size);

      checkFcs = GenerateCrc16 (serial_packet, size);
      delete[] serial_packet;
      return (checkFcs == GetFcs ());
    }
}

void
LoraZigBeeMacTrailer::EnableFcs (bool enable)
{
  m_calcFcs = enable;
  if (!enable)
    {
      m_fcs = 0;
    }
}

bool
LoraZigBeeMacTrailer::IsFcsEnabled (void)
{
  return m_calcFcs;
}

uint16_t
LoraZigBeeMacTrailer::GenerateCrc16 (uint8_t *data, int length)
{
  int i;
  uint16_t accumulator = 0;

  for (i = 0; i < length; ++i)
    {
      accumulator ^= *data;
      accumulator  = (accumulator >> 8) | (accumulator << 8);
      accumulator ^= (accumulator & 0xff00) << 4;
      accumulator ^= (accumulator >> 8) >> 4;
      accumulator ^= (accumulator & 0xff00) >> 5;
      ++data;
    }
  return accumulator;
}

} //namespace ns3
