#ifndef LORA_ZIGBEE_SPECTRUM_SIGNAL_PARAMETERS_H
#define LORA_ZIGBEE_SPECTRUM_SIGNAL_PARAMETERS_H


#include <ns3/spectrum-signal-parameters.h>

namespace ns3 {

class PacketBurst;

/**
 * \ingroup lorazigbee
 *
 * Signal parameters for LoraZigBee.
 */
struct LoraZigBeeSpectrumSignalParameters : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy (void);

  /**
   * default constructor
   */
  LoraZigBeeSpectrumSignalParameters (void);

  /**
   * copy constructor
   * \param p the object to copy from.
   */
  LoraZigBeeSpectrumSignalParameters (const LoraZigBeeSpectrumSignalParameters& p);

  /**
   * The packet burst being transmitted with this signal
   */
  Ptr<PacketBurst> packetBurst;
};

}  // namespace ns3


#endif /* LORA_ZIGBEE_SPECTRUM_SIGNAL_PARAMETERS_H */
