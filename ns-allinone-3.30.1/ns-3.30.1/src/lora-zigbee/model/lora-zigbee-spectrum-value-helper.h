#ifndef LORA_ZIGBEE_SPECTRUM_VALUE_HELPER_H
#define LORA_ZIGBEE_SPECTRUM_VALUE_HELPER_H

#include <ns3/ptr.h>

namespace ns3 {

class SpectrumValue;

/**
 * \ingroup lora-zigbee
 *
 * \brief This class defines all functions to create spectrum model for LoraZigBee
 */
class LoraZigBeeSpectrumValueHelper
{
public:
  LoraZigBeeSpectrumValueHelper (void);
  virtual ~LoraZigBeeSpectrumValueHelper (void);

  /**
   * \brief create spectrum value
   * \param txPower the power transmission in dBm
   * \param channel the channel number per IEEE802.15.4
   * \return a Ptr to a newly created SpectrumValue instance
   */
  Ptr<SpectrumValue> CreateTxPowerSpectralDensity (double txPower, uint32_t channel);

  /**
   * \brief create spectrum value for noise
   * \param channel the channel number per IEEE802.15.4
   * \return a Ptr to a newly created SpectrumValue instance
   */
  Ptr<SpectrumValue> CreateNoisePowerSpectralDensity (uint32_t channel);

  /**
   * \brief total average power of the signal is the integral of the PSD using
   * the limits of the given channel
   * \param psd spectral density
   * \param channel the channel number per IEEE802.15.4
   * \return total power (using composite trap. rule to numerally integrate)
   */
  static double TotalAvgPower (Ptr<const SpectrumValue> psd, uint32_t channel);

private:
  /**
   * A scaling factor for the noise power.
   */
  double m_noiseFactor;

};

} //namespace ns3

#endif /*  LORA_ZIGBEE_SPECTRUM_VALUE_HELPER_H */
