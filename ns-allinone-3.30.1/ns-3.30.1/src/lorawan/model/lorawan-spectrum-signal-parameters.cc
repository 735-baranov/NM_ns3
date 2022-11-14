#include "lorawan-spectrum-signal-parameters.h"
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LoRaWANSpectrumSignalParameters");

LoRaWANSpectrumSignalParameters::LoRaWANSpectrumSignalParameters (void)
{
  NS_LOG_FUNCTION (this);
}

LoRaWANSpectrumSignalParameters::LoRaWANSpectrumSignalParameters (const LoRaWANSpectrumSignalParameters& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  packet = p.packet;
  channelIndex = p.channelIndex;
  dataRateIndex = p.dataRateIndex;
  codeRate = p.codeRate;
}

Ptr<SpectrumSignalParameters>
LoRaWANSpectrumSignalParameters::Copy (void)
{
  NS_LOG_FUNCTION (this);
  return Create<LoRaWANSpectrumSignalParameters> (*this);
}

} // namespace ns3
