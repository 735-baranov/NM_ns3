#ifndef LORA_ZIGBEE_ERROR_MODEL_H
#define LORA_ZIGBEE_ERROR_MODEL_H

#include <ns3/object.h>

namespace ns3 {

/**
 * \ingroup lora-zigbee
 *
 * Model the error rate for IEEE 802.15.4 2.4 GHz AWGN channel for OQPSK
 * the model description can be found in IEEE Std 802.15.4-2006, section
 * E.4.1.7
 */
class LoraZigBeeErrorModel : public Object
{
public:
  /**
   * Get the type ID.
   *
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  LoraZigBeeErrorModel (void);

  /**
   * Return chunk success rate for given SNR.
   *
   * \return success rate (i.e. 1 - chunk error rate)
   * \param snr SNR expressed as a power ratio (i.e. not in dB)
   * \param nbits number of bits in the chunk
   */
  double GetChunkSuccessRate (double snr, uint32_t nbits) const;

private:
  /**
   * Array of precalculated binomial coefficients.
   */
  double m_binomialCoefficients[17];

};


} // namespace ns3

#endif /* LORA_ZIGBEE_ERROR_MODEL_H */
