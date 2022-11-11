#include "lorawan-interference-helper.h"
#include <ns3/spectrum-value.h>
#include <ns3/spectrum-model.h>
#include <ns3/log.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LoRaWANInterferenceHelper");

LoRaWANInterferenceHelper::LoRaWANInterferenceHelper (Ptr<const SpectrumModel> spectrumModel)
  : m_spectrumModel (spectrumModel),
    m_dirty (false)
{
  m_signal = Create<SpectrumValue> (m_spectrumModel);
}

LoRaWANInterferenceHelper::~LoRaWANInterferenceHelper (void)
{
  m_spectrumModel = 0;
  m_signal = 0;
  m_signals.clear ();
}

bool
LoRaWANInterferenceHelper::AddSignal (Ptr<const SpectrumValue> signal)
{
  NS_LOG_FUNCTION (this << signal);

  bool result = false;

  if (signal->GetSpectrumModel () == m_spectrumModel)
    {
      result = m_signals.insert (signal).second;
      if (result && !m_dirty)
        {
          *m_signal += *signal;
        }
    }
  return result;
}

bool
LoRaWANInterferenceHelper::RemoveSignal (Ptr<const SpectrumValue> signal)
{
  NS_LOG_FUNCTION (this << signal);

  bool result = false;

  if (signal->GetSpectrumModel () == m_spectrumModel)
    {
      result = (m_signals.erase (signal) == 1);
      if (result)
        {
          m_dirty = true;
        }
    }
  return result;
}

void
LoRaWANInterferenceHelper::ClearSignals (void)
{
  NS_LOG_FUNCTION (this);

  m_signals.clear ();
  m_dirty = true;
}

Ptr<SpectrumValue>
LoRaWANInterferenceHelper::GetSignalPsd (void) const
{
  NS_LOG_FUNCTION (this);

  if (m_dirty)
    {
      // Sum up the current interference PSD.
      std::set<Ptr<const SpectrumValue> >::const_iterator it;
      m_signal = Create<SpectrumValue> (m_spectrumModel);
      for (it = m_signals.begin (); it != m_signals.end (); ++it)
        {
          *m_signal += *(*it);
        }
      m_dirty = false;
    }

  return m_signal->Copy ();
}

}
