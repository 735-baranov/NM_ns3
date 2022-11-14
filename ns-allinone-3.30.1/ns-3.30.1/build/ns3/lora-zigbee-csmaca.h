#ifndef LORA_ZIGBEE_CSMACA_H
#define LORA_ZIGBEE_CSMACA_H

#include <ns3/object.h>
#include <ns3/event-id.h>
#include <ns3/lora-zigbee-mac.h>

namespace ns3 {

class UniformRandomVariable;

/**
 * \ingroup lora-zigbee
 *
 * This method informs the MAC  whether the channel is idle or busy.
 */
typedef Callback<void, LoraZigBeeMacState> LoraZigBeeMacStateCallback;

/**
 * \ingroup lora-zigbee
 *
 * This class is a helper for the LoraZigBeeMac to manage the Csma/CA
 * state machine according to IEEE 802.15.4-2006, section 7.5.1.4.
 */
class LoraZigBeeCsmaCa : public Object
{

public:
  /**
   * Get the type ID.
   *
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Default constructor.
   */
  LoraZigBeeCsmaCa (void);
  virtual ~LoraZigBeeCsmaCa (void);

  /**
   * Set the MAC to which this CSMA/CA implementation is attached to.
   *
   * \param mac the used MAC
   */
  void SetMac (Ptr<LoraZigBeeMac> mac);

  /**
   * Get the MAC to which this CSMA/CA implementation is attached to.
   *
   * \return the used MAC
   */
  Ptr<LoraZigBeeMac> GetMac (void) const;

  /**
   * Configure for the use of the slotted CSMA/CA version.
   */
  void SetSlottedCsmaCa (void);

  /**
   * Configure for the use of the unslotted CSMA/CA version.
   */
  void SetUnSlottedCsmaCa (void);

  /**
   * Check if the slotted CSMA/CA version is being used.
   *
   * \return true, if slotted CSMA/CA is used, false otherwise.
   */
  bool IsSlottedCsmaCa (void) const;

  /**
   * Check if the unslotted CSMA/CA version is being used.
   *
   * \return true, if unslotted CSMA/CA is used, false otherwise.
   */
  bool IsUnSlottedCsmaCa (void) const;

  /**
   * Set the minimum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \param macMinBE the minimum backoff exponent value
   */
  void SetMacMinBE (uint8_t macMinBE);

  /**
   * Get the minimum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \return the minimum backoff exponent value
   */
  uint8_t GetMacMinBE (void) const;

  /**
   * Set the maximum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \param macMaxBE the maximum backoff exponent value
   */
  void SetMacMaxBE (uint8_t macMaxBE);

  /**
   * Get the maximum backoff exponent value.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \return the maximum backoff exponent value
   */
  uint8_t GetMacMaxBE (void) const;

  /**
   * Set the maximum number of backoffs.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \param macMaxCSMABackoffs the maximum number of backoffs
   */
  void SetMacMaxCSMABackoffs (uint8_t macMaxCSMABackoffs);

  /**
   * Get the maximum number of backoffs.
   * See IEEE 802.15.4-2006, section 7.4.2, Table 86.
   *
   * \return the maximum number of backoffs
   */
  uint8_t GetMacMaxCSMABackoffs (void) const;

  /**
   * Set the number of symbols forming the basic time period used by the
   * CSMA-CA algorithm.
   * See IEEE 802.15.4-2006, section 7.4.1, Table 85.
   *
   * \param unitBackoffPeriod the period length in symbols
   */
  void SetUnitBackoffPeriod (uint64_t unitBackoffPeriod);

  /**
   * Get the number of symbols forming the basic time period used by the
   * CSMA-CA algorithm.
   * See IEEE 802.15.4-2006, section 7.4.1, Table 85.
   *
   * \return the period length in symbols
   */
  uint64_t GetUnitBackoffPeriod (void) const;

  /**
   * Get the amount of time from now to the beginning of the next slot.
   *
   * \return time offset to the next slot
   */
  Time GetTimeToNextSlot (void) const;

  /**
   * Start CSMA-CA algorithm (step 1), initialize NB, BE for both slotted and unslotted
   * CSMA-CA. For the slotted initialize CW plus also start on the backoff boundary
   */
  void Start (void);

  /**
   * Cancel CSMA-CA algorithm.
   */
  void Cancel (void);

  /**
   * In step 2 of the CSMA-CA, perform a random backoff in the range of 0 to 2^BE -1
   */
  void RandomBackoffDelay (void);

  /**
   * In the slotted CSMA-CA, after random backoff, determine if the remaining
   * CSMA-CA operation can proceed, i.e. can the entire transactions can be
   * transmitted before the end of the CAP. This step is performed between step
   * 2 and 3. This step is NOT performed for the unslotted CSMA-CA. If it can
   * proceed function RequestCCA() is called.
   */
  void CanProceed (void);

  /**
   * Request the Phy to perform CCA (Step 3)
   */
  void RequestCCA (void);

  /**
   *  IEEE 802.15.4-2006 section 6.2.2.2
   *  PLME-CCA.confirm status
   *  @param status TRX_OFF, BUSY or IDLE
   *
   * When Phy has completed CCA, it calls back here which in turn execute the final steps
   * of the CSMA-CA algorithm.
   * It checks to see if the Channel is idle, if so check the Contention window  before
   * permitting transmission (step 5). If channel is busy, either backoff and perform CCA again or
   * treat as channel access failure (step 4).
   */
  void PlmeCcaConfirm (LoraZigBeePhyEnumeration status);

  /**
   * Set the callback function to the MAC. Used at the end of a Channel Assessment, as part of the
   * interconnections between the CSMA-CA and the MAC. The callback
   * lets MAC know a channel is either idle or busy.
   *
   * \param macState the mac state callback
   */
  void SetLoraZigBeeMacStateCallback (LoraZigBeeMacStateCallback macState);

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams that have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

  /**
   * Get the number of CSMA retries
   *
   * \returns the number of CSMA retries
   */
  uint8_t GetNB (void);

private:
  // Disable implicit copy constructors
  /**
   * \brief Copy constructor - defined and not implemented.
   */
  LoraZigBeeCsmaCa (LoraZigBeeCsmaCa const &);
  /**
   * \brief Copy constructor - defined and not implemented.
   * \returns
   */
  LoraZigBeeCsmaCa& operator= (LoraZigBeeCsmaCa const &);
 
  virtual void DoDispose (void);

  /**
   * The callback to inform the configured MAC of the CSMA/CA result.
   */
  LoraZigBeeMacStateCallback m_lorazigbeeMacStateCallback;

  /**
   * Beacon-enabled slotted or nonbeacon-enabled unslotted CSMA-CA.
   */
  bool m_isSlotted;

  /**
   * The MAC instance for which this CSMAÄ/CA implementation is configured.
   */
  Ptr<LoraZigBeeMac> m_mac;

  /**
   * Number of backoffs for the current transmission.
   */
  uint8_t m_NB;

  /**
   * Contention window length (used in slotted ver only).
   */
  uint8_t m_CW;

  /**
   * Backoff exponent.
   */
  uint8_t m_BE;

  /**
   * Battery Life Extension.
   */
  bool m_BLE;

  /**
   * Minimum backoff exponent. 0 - macMaxBE, default 3
   */
  uint8_t m_macMinBE;                   //

  /**
   * Maximum backoff exponent. 3 - 8, default 5
   */
  uint8_t m_macMaxBE;

  /**
   * Maximum number of backoffs. 0 - 5, default 4
   */
  uint8_t m_macMaxCSMABackoffs;

  /**
   * Number of symbols per CSMA/CA time unit, default 20 symbols.
   */
  uint64_t m_aUnitBackoffPeriod;

  /**
   * Uniform random variable stream.
   */
  Ptr<UniformRandomVariable> m_random;

  /**
   * Scheduler event for the start of the next random backoff/slot.
   */
  EventId m_randomBackoffEvent;

  /**
   * Scheduler event when to start the CCA after a random backoff.
   */
  EventId m_requestCcaEvent;

  /**
   * Scheduler event for checking if we can complete the transmission before the
   * end of the CAP.
   */
  EventId m_canProceedEvent;

  /**
   * Flag indicating that the PHY is currently running a CCA. Used to prevent
   * reporting the channel status to the MAC while canceling the CSMA algorithm.
   */
  bool m_ccaRequestRunning;
};

}

// namespace ns-3

#endif /* LORA_ZIGBEE_CSMACA_H */
