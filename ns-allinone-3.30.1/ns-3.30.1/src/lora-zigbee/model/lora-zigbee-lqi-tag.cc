#include "lora-zigbee-lqi-tag.h"
#include <ns3/integer.h>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LoraZigBeeLqiTag);

TypeId
LoraZigBeeLqiTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LoraZigBeeLqiTag")
    .SetParent<Tag> ()
    .SetGroupName ("LoraZigBee")
    .AddConstructor<LoraZigBeeLqiTag> ()
    .AddAttribute ("Lqi", "The lqi of the last packet received",
                   IntegerValue (0),
                   MakeIntegerAccessor (&LoraZigBeeLqiTag::Get),
                   MakeIntegerChecker<uint8_t> ())
  ;
  return tid;
}

TypeId
LoraZigBeeLqiTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

LoraZigBeeLqiTag::LoraZigBeeLqiTag (void)
  : m_lqi (0)
{
}

LoraZigBeeLqiTag::LoraZigBeeLqiTag (uint8_t lqi)
  : m_lqi (lqi)
{
}

uint32_t
LoraZigBeeLqiTag::GetSerializedSize (void) const
{
  return sizeof (uint8_t);
}

void
LoraZigBeeLqiTag::Serialize (TagBuffer i) const
{
  i.WriteU8 (m_lqi);
}

void
LoraZigBeeLqiTag::Deserialize (TagBuffer i)
{
  m_lqi = i.ReadU8 ();
}

void
LoraZigBeeLqiTag::Print (std::ostream &os) const
{
  os << "Lqi = " << m_lqi;
}

void
LoraZigBeeLqiTag::Set (uint8_t lqi)
{
  m_lqi = lqi;
}

uint8_t
LoraZigBeeLqiTag::Get (void) const
{
  return m_lqi;
}

}
