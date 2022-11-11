#ifndef LR_WPAN_SPECTRUM_SIGNAL_PARAMETERS_H
#define LR_WPAN_SPECTRUM_SIGNAL_PARAMETERS_H


#include <ns3/spectrum-signal-parameters.h>

namespace ns3 {

class PacketBurst;

/**
 * \ingroup lr-wpan
 *
 * Signal parameters for LrWpan.
 */
struct LrWpanSpectrumSignalParameters : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy (void);

  /**
   * default constructor
   */
  LrWpanSpectrumSignalParameters (void);

  /**
   * copy constructor
   * \param p the object to copy from.
   */
  LrWpanSpectrumSignalParameters (const LrWpanSpectrumSignalParameters& p);

  /**
   * The packet burst being transmitted with this signal
   */
  Ptr<PacketBurst> packetBurst;
};

}  // namespace ns3


#endif /* LR_WPAN_SPECTRUM_SIGNAL_PARAMETERS_H */
