/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef LORA_ZIGBEE_H
#define LORA_ZIGBEE_H
#include <ns3/uinteger.h>
#include <ns3/packet.h>
#include <ns3/flow-id-tag.h>

#include <vector>

// For confirmed messages: number of transmissions
#define DEFAULT_NUMBER_US_TRANSMISSIONS 4
#define DEFAULT_NUMBER_DS_TRANSMISSIONS DEFAULT_NUMBER_US_TRANSMISSIONS

// Default settings for EU863-870
#define RECEIVE_DELAY1 1000000 // in uS
#define RECEIVE_DELAY2 2000000 // in uS

namespace ns3 {

/* ... */

  /**
   * \ingroup lorazigbee
   *
   * LoRaZiGbee channels
   */
  typedef struct
  {
    uint8_t m_channelIndex;
    uint32_t m_fc; // in Hz
    uint32_t m_bw;
    uint8_t m_subBandIndex;
  } LoRaZiGbeeChannel;

  /**
   * \ingroup lorazigbee
   *
   * LoRa spreading factors
   */
  typedef enum
  {
   LORAZiGbee_SF6 = 6,
   LORAZiGbee_SF7,
   LORAZiGbee_SF8,
   LORAZiGbee_SF9,
   LORAZiGbee_SF10,
   LORAZiGbee_SF11,
   LORAZiGbee_SF12,
  } LoRaSpreadingFactor;

  /**
   * \ingroup lorazigbee
   *
   * LoRaZiGbee Data Rate per $7.1.3
   */
  typedef struct
  {
    uint8_t dataRateIndex;
    LoRaSpreadingFactor spreadingFactor;
    uint32_t bandWith;
  } LoRaZiGbeeDataRate;


  /**
   * \ingroup lorazigbee
   *
   * LoRaZiGbee message types as per $4.2.1 in LoRaZiGbee spec
   */
  typedef enum
  {
    LORAZiGbee_DT_GATEWAY = 0,
    LORAZiGbee_DT_END_DEVICE_CLASS_A,
    LORAZiGbee_DT_END_DEVICE_CLASS_B,
    LORAZiGbee_DT_END_DEVICE_CLASS_C
  } LoRaZiGbeeDeviceType;

  /**
   * \ingroup lorazigbee
   *
   * LoRaZiGbee message types as per $4.2.1 in LoRaZiGbee spec
   */
  typedef enum
  {
   LORAZiGbee_JOIN_REQUEST = 0,
   LORAZiGbee_JOIN_ACCEPT,
   LORAZiGbee_UNCONFIRMED_DATA_UP,
   LORAZiGbee_UNCONFIRMED_DATA_DOWN,
   LORAZiGbee_CONFIRMED_DATA_UP,
   LORAZiGbee_CONFIRMED_DATA_DOWN,
   LORAZiGbee_RFU,
   LORAZiGbee_PROPRIETARY,
  } LoRaZiGbeeMsgType;

  class LoRaZiGbee {

  public:
    /**
     * The supported LoRa channels
     */
    static const std::vector<LoRaZiGbeeChannel> m_supportedChannels;

    /**
     * The supported LoRaZiGbee data rates
     */
    static const std::vector<LoRaZiGbeeDataRate> m_supportedDataRates;

    /*
     * Get the RX1 receive window data rate, note frequency is always the same for RW1
     */
    static uint8_t GetRX1DataRateIndex (uint8_t upstreamDRIndex, uint8_t rx1DROffset);

    /**
     * The channel and data rate index for transmissions in the second receive
     * window (RW2) of a class A end device
     */
    static uint8_t m_RW2ChannelIndex;
    static uint8_t m_RW2DataRateIndex;

  }; // class LoRaZiGbee

  class LoRaZiGbeeMsgTypeTag : public Tag {
  public:
    LoRaZiGbeeMsgTypeTag (void);

    void SetMsgType (LoRaZiGbeeMsgType);
    LoRaZiGbeeMsgType GetMsgType (void) const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);

    // inherited function, no need to doc.
    virtual TypeId GetInstanceTypeId (void) const;

    // inherited function, no need to doc.
    virtual uint32_t GetSerializedSize (void) const;

    // inherited function, no need to doc.
    virtual void Serialize (TagBuffer i) const;

    // inherited function, no need to doc.
    virtual void Deserialize (TagBuffer i);

    // inherited function, no need to doc.
    virtual void Print (std::ostream &os) const;
  private:
    LoRaZiGbeeMsgType m_msgType;
  }; // class LoRaZiGbeeMsgTypeTag

  class LoRaZiGbeePhyParamsTag : public Tag {
  public:
    LoRaZiGbeePhyParamsTag (void);

    void SetChannelIndex (uint8_t);
    uint8_t GetChannelIndex (void) const;

    void SetDataRateIndex (uint8_t);
    uint8_t GetDataRateIndex (void) const;

    void SetCodeRate (uint8_t);
    uint8_t GetCodeRate (void) const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);

    // inherited function, no need to doc.
    virtual TypeId GetInstanceTypeId (void) const;

    // inherited function, no need to doc.
    virtual uint32_t GetSerializedSize (void) const;

    // inherited function, no need to doc.
    virtual void Serialize (TagBuffer i) const;

    // inherited function, no need to doc.
    virtual void Deserialize (TagBuffer i);

    // inherited function, no need to doc.
    virtual void Print (std::ostream &os) const;
  private:
    uint8_t m_channelIndex;
    uint8_t m_dataRateIndex;
    uint8_t m_codeRate;
  }; // class LoRaZiGbeePhyParamsTag

  typedef FlowIdTag LoRaZiGbeePhyTraceIdTag;

  class LoRaZiGbeeCounterSingleton {
  public:
    LoRaZiGbeeCounterSingleton ();
    //static LoRaZiGbeeCounterSingleton* GetPtr ();
    static uint64_t GetCounter () { return m_counter--; } // return current value and decrement afterwards
  private:
    static LoRaZiGbeeCounterSingleton* m_ptr;
    static uint64_t m_counter;
  }; // class LoRaZiGbeeCounterSingleton

} // namespace ns3
#endif /* LORA_ZIGBEE_H */

