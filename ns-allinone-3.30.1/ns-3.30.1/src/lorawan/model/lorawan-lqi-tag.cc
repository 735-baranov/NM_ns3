#include "lorawan-lqi-tag.h"
#include <ns3/integer.h>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LoRaWANLqiTag);

TypeId
LoRaWANLqiTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LoRaWANLqiTag")
    .SetParent<Tag> ()
    .SetGroupName ("LoRaWAN")
    .AddConstructor<LoRaWANLqiTag> ()
    .AddAttribute ("Lqi", "The lqi of the last packet received",
                   IntegerValue (0),
                   MakeIntegerAccessor (&LoRaWANLqiTag::Get),
                   MakeIntegerChecker<uint8_t> ())
  ;
  return tid;
}

TypeId
LoRaWANLqiTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

LoRaWANLqiTag::LoRaWANLqiTag (void)
  : m_lqi (0)
{
}

LoRaWANLqiTag::LoRaWANLqiTag (uint8_t lqi)
  : m_lqi (lqi)
{
}

uint32_t
LoRaWANLqiTag::GetSerializedSize (void) const
{
  return sizeof (uint8_t);
}

void
LoRaWANLqiTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_lqi);
}

void
LoRaWANLqiTag::Deserialize (TagBuffer i)
{
  m_lqi = i.ReadU8 ();
}

void
LoRaWANLqiTag::Print (std::ostream &os) const
{
  os << "Lqi = " << m_lqi;
}

void
LoRaWANLqiTag::Set (uint8_t lqi)
{
  m_lqi = lqi;
}

uint8_t
LoRaWANLqiTag::Get (void) const
{
  return m_lqi;
}

}
