#ifndef LORA_ZIGBEE_FRAME_HEADER_H
#define LORA_ZIGBEE_FRAME_HEADER_H

#include <ns3/header.h>
#include "ns3/ipv4-address.h"

#define LORA_ZIGBEE_FHDR_ADR_MASK 0x80
#define LORA_ZIGBEE_FHDR_ADRACKREQ_MASK 0x40
#define LORA_ZIGBEE_FHDR_ACK_MASK 0x20
#define LORA_ZIGBEE_FHDR_FPENDING_MASK 0x10
#define LORA_ZIGBEE_FHDR_FOPTSLEN_MASK 0x0F

namespace ns3 {

/**
 * \ingroup lorazigbee
 * Represent the Frame Header (FHDR) in LoRaZiGbee
 */
class LoRaZiGbeeFrameHeader : public Header
{
public:
  LoRaZiGbeeFrameHeader (void);

  LoRaZiGbeeFrameHeader (Ipv4Address devAddr, bool adr, bool adrAckReq, bool ack, bool framePending, uint8_t FOptsLen, uint16_t frameCounter, uint16_t framePort);
  ~LoRaZiGbeeFrameHeader (void);

  Ipv4Address getDevAddr(void) const;
  void setDevAddr(Ipv4Address);

  bool getAck() const;
  void setAck(bool);

  bool getFramePending() const;
  void setFramePending(bool);

  uint16_t getFrameCounter() const;
  void setFrameCounter(uint16_t);

  uint8_t getFramePort() const;
  void setFramePort(uint8_t);

  bool getSerializeFramePort() const;
  void setSerializeFramePort(bool);

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  void Print (std::ostream &os) const;
  uint32_t GetSerializedSize (void) const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);

  bool IsAck() const;
  bool IsFramePending() const;

private:
  Ipv4Address m_devAddr; //!< Short device address of end-device
  uint8_t m_frameControl;
  uint16_t m_frameCounter;
  uint8_t m_frameOptions[15];
  uint8_t m_framePort; // Not actually part of the frame header, but we include it here for ease of use
  bool m_serializeFramePort;
}; //LoRaZiGbeeFrameHeader

}; // namespace ns-3

#endif /* LORA_ZIGBEE_FRAME_HEADER_H */
