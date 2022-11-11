#include "lora-zigbee-spectrum-value-helper.h"
#include <ns3/log.h>
#include <ns3/spectrum-value.h>

#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LoraZigBeeSpectrumValueHelper");

Ptr<SpectrumModel> g_LoraZigBeeSpectrumModel; //!< Global object used to initialize the LoraZigBee Spectrum Model

/**
 * \ingroup lora-zigbee
 * \brief Helper class used to automatically initialize the LoraZigBee Spectrum Model objects
 */
class LoraZigBeeSpectrumModelInitializer
{
public:
  LoraZigBeeSpectrumModelInitializer (void)
  {
    NS_LOG_FUNCTION (this);

    Bands bands;
    // 1 MHz resolution, with center frequency of 2400, 2401, ... 2483
    // overall frequency span of 2399.5 MHz through 2483.5 MHz (83 bands)
    for (int i = -1; i < 83; i++)
      {
        BandInfo bi;
        bi.fl = 2400.5e6 + i * 1.0e6;
        bi.fh = 2400.5e6 + (i + 1) * 1.0e6;
        bi.fc = (bi.fl +  bi.fh) / 2;
        bands.push_back (bi);
      }
    g_LoraZigBeeSpectrumModel = Create<SpectrumModel> (bands);
  }

} g_LoraZigBeeSpectrumModelInitializerInstance; //!< Global object used to initialize the LoraZigBee Spectrum Model

LoraZigBeeSpectrumValueHelper::LoraZigBeeSpectrumValueHelper (void)
{
  NS_LOG_FUNCTION (this);
  m_noiseFactor = 1.0;
}

LoraZigBeeSpectrumValueHelper::~LoraZigBeeSpectrumValueHelper (void)
{
  NS_LOG_FUNCTION (this);
}

Ptr<SpectrumValue>
LoraZigBeeSpectrumValueHelper::CreateTxPowerSpectralDensity (double txPower, uint32_t channel)
{
  NS_LOG_FUNCTION (this);
  Ptr<SpectrumValue> txPsd = Create <SpectrumValue> (g_LoraZigBeeSpectrumModel);

  // txPower is expressed in dBm. We must convert it into natural unit (W).
  txPower = pow (10., (txPower - 30) / 10);

  // The effective occupied bandwidth of the signal is modelled to be 2 MHz.
  // 99.5% of power is within +/- 1MHz of center frequency, and 0.5% is outside.
  // There are 5 bands containing signal power.  The middle (center) band
  // contains half of the power.  The two inner side bands contain 49.5%.
  // The two outer side bands contain roughly 0.5%.
  double txPowerDensity = txPower / 2.0e6;

  NS_ASSERT_MSG ((channel >= 11 && channel <= 26), "Invalid channel numbers");

  // The channel assignment is in section 6.1.2.1
  // Channel 11 centered at 2.405 GHz, 12 at 2.410 GHz, ... 26 at 2.480 GHz
  (*txPsd)[2405 + 5 * (channel - 11) - 2400 - 2] = txPowerDensity * 0.005;
  (*txPsd)[2405 + 5 * (channel - 11) - 2400 - 1] = txPowerDensity * 0.495;
  (*txPsd)[2405 + 5 * (channel - 11) - 2400] = txPowerDensity; // center
  (*txPsd)[2405 + 5 * (channel - 11) - 2400 + 1 ] = txPowerDensity * 0.495;
  (*txPsd)[2405 + 5 * (channel - 11) - 2400 + 2 ] = txPowerDensity * 0.005;

  // If more power is allocated to more subbands in future revisions of
  // this model, make sure to renormalize so that the integral of the
  // txPsd still equals txPower

  return txPsd;
}

Ptr<SpectrumValue>
LoraZigBeeSpectrumValueHelper::CreateNoisePowerSpectralDensity (uint32_t channel)
{
  NS_LOG_FUNCTION (this);
  Ptr<SpectrumValue> noisePsd = Create <SpectrumValue> (g_LoraZigBeeSpectrumModel);

  static const double BOLTZMANN = 1.3803e-23;
  // Nt  is the power of thermal noise in W
  double Nt = BOLTZMANN * 290.0;
  // noise Floor (W) which accounts for thermal noise and non-idealities of the receiver
  double noisePowerDensity = m_noiseFactor * Nt;

  NS_ASSERT_MSG ((channel >= 11 && channel <= 26), "Invalid channel numbers");

  (*noisePsd)[2405 + 5 * (channel - 11) - 2400 - 2] = noisePowerDensity;
  (*noisePsd)[2405 + 5 * (channel - 11) - 2400 - 1] = noisePowerDensity;
  (*noisePsd)[2405 + 5 * (channel - 11) - 2400] = noisePowerDensity;
  (*noisePsd)[2405 + 5 * (channel - 11) - 2400 + 1] = noisePowerDensity;
  (*noisePsd)[2405 + 5 * (channel - 11) - 2400 + 2] = noisePowerDensity;

  return noisePsd;
}

double
LoraZigBeeSpectrumValueHelper::TotalAvgPower (Ptr<const SpectrumValue> psd, uint32_t channel)
{
  NS_LOG_FUNCTION (psd);
  double totalAvgPower = 0.0;

  NS_ASSERT (psd->GetSpectrumModel () == g_LoraZigBeeSpectrumModel);

  // numerically integrate to get area under psd using 1 MHz resolution

  totalAvgPower += (*psd)[2405 + 5 * (channel - 11) - 2400 - 2];
  totalAvgPower += (*psd)[2405 + 5 * (channel - 11) - 2400 - 1];
  totalAvgPower += (*psd)[2405 + 5 * (channel - 11) - 2400];
  totalAvgPower += (*psd)[2405 + 5 * (channel - 11) - 2400 + 1];
  totalAvgPower += (*psd)[2405 + 5 * (channel - 11) - 2400 + 2];
  totalAvgPower *= 1.0e6;

  return totalAvgPower;
}

} // namespace ns3
