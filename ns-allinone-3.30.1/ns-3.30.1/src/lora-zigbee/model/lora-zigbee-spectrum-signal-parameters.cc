#include "lora-zigbee-spectrum-signal-parameters.h"
#include <ns3/log.h>
#include <ns3/packet-burst.h>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LoraZigBeeSpectrumSignalParameters");

LoraZigBeeSpectrumSignalParameters::LoraZigBeeSpectrumSignalParameters (void)
{
  NS_LOG_FUNCTION (this);
}

LoraZigBeeSpectrumSignalParameters::LoraZigBeeSpectrumSignalParameters (const LoraZigBeeSpectrumSignalParameters& p)
  : SpectrumSignalParameters (p)
{
  NS_LOG_FUNCTION (this << &p);
  packetBurst = p.packetBurst->Copy ();
}

Ptr<SpectrumSignalParameters>
LoraZigBeeSpectrumSignalParameters::Copy (void)
{
  NS_LOG_FUNCTION (this);
  return Create<LoraZigBeeSpectrumSignalParameters> (*this);
}

} // namespace ns3
