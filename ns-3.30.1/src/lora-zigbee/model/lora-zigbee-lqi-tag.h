#ifndef LORA_ZIGBEE_LQI_TAG_H
#define LORA_ZIGBEE_LQI_TAG_H

#include <ns3/tag.h>

namespace ns3 {

class LoraZigBeeLqiTag : public Tag
{
public:
  /**
   * Get the type ID.
   *
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Create a LoraZigBeeLqiTag with the default LQI 0.
   */
  LoraZigBeeLqiTag (void);

  /**
   * Create a LoraZigBeeLqiTag with the given LQI value.
   * \param lqi The LQI.
   */
  LoraZigBeeLqiTag (uint8_t lqi);

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  /**
   * Set the LQI to the given value.
   *
   * \param lqi the value of the LQI to set
   */
  void Set (uint8_t lqi);

  /**
   * Get the LQI value.
   *
   * \return the LQI value
   */
  uint8_t Get (void) const;
private:
  /**
   * The current LQI value of the tag.
   */
  uint8_t m_lqi;
};


}
#endif /* LORA_ZIGBEE_LQI_TAG_H */
