#include "lora-zigbee-csmaca.h"
#include <ns3/random-variable-stream.h>
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LoraZigBeeCsmaCa");

NS_OBJECT_ENSURE_REGISTERED (LoraZigBeeCsmaCa);

TypeId
LoraZigBeeCsmaCa::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LoraZigBeeCsmaCa")
    .SetParent<Object> ()
    .SetGroupName ("LoraZigBee")
    .AddConstructor<LoraZigBeeCsmaCa> ()
  ;
  return tid;
}

LoraZigBeeCsmaCa::LoraZigBeeCsmaCa ()
{
  // TODO-- make these into ns-3 attributes

  m_isSlotted = false;
  m_NB = 0;
  m_CW = 2;
  m_BLE = false;
  m_macMinBE = 3;
  m_macMaxBE = 5;
  m_macMaxCSMABackoffs = 4;
  m_aUnitBackoffPeriod = 20; //20 symbols
  m_random = CreateObject<UniformRandomVariable> ();
  m_BE = m_macMinBE;
  m_ccaRequestRunning = false;
}

LoraZigBeeCsmaCa::~LoraZigBeeCsmaCa ()
{
  m_mac = 0;
}

void
LoraZigBeeCsmaCa::DoDispose ()
{
  m_lorazigbeeMacStateCallback = MakeNullCallback< void, LoraZigBeeMacState> ();
  Cancel ();
  m_mac = 0;
}

void
LoraZigBeeCsmaCa::SetMac (Ptr<LoraZigBeeMac> mac)
{
  m_mac = mac;
}

Ptr<LoraZigBeeMac>
LoraZigBeeCsmaCa::GetMac (void) const
{
  return m_mac;
}

void
LoraZigBeeCsmaCa::SetSlottedCsmaCa (void)
{
  NS_LOG_FUNCTION (this);
  m_isSlotted = true;
}

void
LoraZigBeeCsmaCa::SetUnSlottedCsmaCa (void)
{
  NS_LOG_FUNCTION (this);
  m_isSlotted = false;
}

bool
LoraZigBeeCsmaCa::IsSlottedCsmaCa (void) const
{
  NS_LOG_FUNCTION (this);
  return (m_isSlotted);
}

bool
LoraZigBeeCsmaCa::IsUnSlottedCsmaCa (void) const
{
  NS_LOG_FUNCTION (this);
  return (!m_isSlotted);
}

void
LoraZigBeeCsmaCa::SetMacMinBE (uint8_t macMinBE)
{
  NS_LOG_FUNCTION (this << macMinBE);
  m_macMinBE = macMinBE;
}

uint8_t
LoraZigBeeCsmaCa::GetMacMinBE (void) const
{
  NS_LOG_FUNCTION (this);
  return m_macMinBE;
}

void
LoraZigBeeCsmaCa::SetMacMaxBE (uint8_t macMaxBE)
{
  NS_LOG_FUNCTION (this << macMaxBE);
  m_macMinBE = macMaxBE;
}

uint8_t
LoraZigBeeCsmaCa::GetMacMaxBE (void) const
{
  NS_LOG_FUNCTION (this);
  return m_macMaxBE;
}

void
LoraZigBeeCsmaCa::SetMacMaxCSMABackoffs (uint8_t macMaxCSMABackoffs)
{
  NS_LOG_FUNCTION (this << macMaxCSMABackoffs);
  m_macMaxCSMABackoffs = macMaxCSMABackoffs;
}

uint8_t
LoraZigBeeCsmaCa::GetMacMaxCSMABackoffs (void) const
{
  NS_LOG_FUNCTION (this);
  return m_macMaxCSMABackoffs;
}

void
LoraZigBeeCsmaCa::SetUnitBackoffPeriod (uint64_t unitBackoffPeriod)
{
  NS_LOG_FUNCTION (this << unitBackoffPeriod);
  m_aUnitBackoffPeriod = unitBackoffPeriod;
}

uint64_t
LoraZigBeeCsmaCa::GetUnitBackoffPeriod (void) const
{
  NS_LOG_FUNCTION (this);
  return m_aUnitBackoffPeriod;
}

Time
LoraZigBeeCsmaCa::GetTimeToNextSlot (void) const
{
  NS_LOG_FUNCTION (this);

  // TODO: Calculate the offset to the next slot.

  return Seconds (0);

}
void
LoraZigBeeCsmaCa::Start ()

{
  NS_LOG_FUNCTION (this);
  m_NB = 0;
  if (IsSlottedCsmaCa ())
    {
      m_CW = 2;
      if (m_BLE)
        {
          m_BE = std::min (static_cast<uint8_t> (2), m_macMinBE);
        }
      else
        {
          m_BE = m_macMinBE;
        }
      //TODO: for slotted, locate backoff period boundary. i.e. delay to the next slot boundary
      Time backoffBoundary = GetTimeToNextSlot ();
      m_randomBackoffEvent = Simulator::Schedule (backoffBoundary, &LoraZigBeeCsmaCa::RandomBackoffDelay, this);
    }
  else
    {
      m_BE = m_macMinBE;
      m_randomBackoffEvent = Simulator::ScheduleNow (&LoraZigBeeCsmaCa::RandomBackoffDelay, this);
    }
  /*
  *  TODO: If using Backoff.cc (will need to modify Backoff::GetBackoffTime)
  *        Backoff.m_minSlots = 0;
  *        Backoff.m_ceiling = m_BE;
  *        Backoff.ResetBackoffTime(); //m_NB is same as m_numBackoffRetries in Backoff.h
  *        Backoff.m_maxRetries = macMaxCSMABackoffs;
  *        Backoff.m_slotTime = m_backoffPeriod;
  */
}

void
LoraZigBeeCsmaCa::Cancel ()
{
  m_randomBackoffEvent.Cancel ();
  m_requestCcaEvent.Cancel ();
  m_canProceedEvent.Cancel ();
}

/*
 * Delay for backoff period in the range 0 to 2^BE -1 units
 * TODO: If using Backoff.cc (Backoff::GetBackoffTime) will need to be slightly modified
 */
void
LoraZigBeeCsmaCa::RandomBackoffDelay ()
{
  NS_LOG_FUNCTION (this);

  uint64_t upperBound = (uint64_t) pow (2, m_BE) - 1;
  uint64_t backoffPeriod;
  Time randomBackoff;
  uint64_t symbolRate;
  bool isData = false;


  symbolRate = (uint64_t) m_mac->GetPhy ()->GetDataOrSymbolRate (isData); //symbols per second
  backoffPeriod = (uint64_t)m_random->GetValue (0, upperBound+1); // num backoff periods
  randomBackoff = MicroSeconds (backoffPeriod * GetUnitBackoffPeriod () * 1000 * 1000 / symbolRate);

  if (IsUnSlottedCsmaCa ())
    {
      NS_LOG_LOGIC ("Unslotted:  requesting CCA after backoff of " << randomBackoff.GetMicroSeconds () << " us");
      m_requestCcaEvent = Simulator::Schedule (randomBackoff, &LoraZigBeeCsmaCa::RequestCCA, this);
    }
  else
    {
      NS_LOG_LOGIC ("Slotted:  proceeding after backoff of " << randomBackoff.GetMicroSeconds () << " us");
      m_canProceedEvent = Simulator::Schedule (randomBackoff, &LoraZigBeeCsmaCa::CanProceed, this);
    }
}

// TODO : Determine if transmission can be completed before end of CAP for the slotted csmaca
//        If not delay to the next CAP
void
LoraZigBeeCsmaCa::CanProceed ()
{
  NS_LOG_FUNCTION (this);

  bool canProceed = true;

  if (m_BLE)
    {
    }
  else
    {
    }

  if (canProceed)
    {
      // TODO: For slotted, Perform CCA on backoff period boundary i.e. delay to next slot boundary
      Time backoffBoundary = GetTimeToNextSlot ();
      m_requestCcaEvent = Simulator::Schedule (backoffBoundary, &LoraZigBeeCsmaCa::RequestCCA, this);
    }
  else
    {
      Time nextCap = Seconds (0);
      m_randomBackoffEvent = Simulator::Schedule (nextCap, &LoraZigBeeCsmaCa::RandomBackoffDelay, this);
    }
}

void
LoraZigBeeCsmaCa::RequestCCA ()
{
  NS_LOG_FUNCTION (this);
  m_ccaRequestRunning = true;
  m_mac->GetPhy ()->PlmeCcaRequest ();
}

/*
 * This function is called when the phy calls back after completing a PlmeCcaRequest
 */
void
LoraZigBeeCsmaCa::PlmeCcaConfirm (LoraZigBeePhyEnumeration status)
{
  NS_LOG_FUNCTION (this << status);

  // Only react on this event, if we are actually waiting for a CCA.
  // If the CSMA algorithm was canceled, we could still receive this event from
  // the PHY. In this case we ignore the event.
  if (m_ccaRequestRunning)
    {
      m_ccaRequestRunning = false;
      if (status == IEEE_802_15_4_PHY_IDLE)
        {
          if (IsSlottedCsmaCa ())
            {
              m_CW--;
              if (m_CW == 0)
                {
                  // inform MAC channel is idle
                  if (!m_lorazigbeeMacStateCallback.IsNull ())
                    {
                      NS_LOG_LOGIC ("Notifying MAC of idle channel");
                      m_lorazigbeeMacStateCallback (CHANNEL_IDLE);
                    }
                }
              else
                {
                  NS_LOG_LOGIC ("Perform CCA again, m_CW = " << m_CW);
                  m_requestCcaEvent = Simulator::ScheduleNow (&LoraZigBeeCsmaCa::RequestCCA, this); // Perform CCA again
                }
            }
          else
            {
              // inform MAC, channel is idle
              if (!m_lorazigbeeMacStateCallback.IsNull ())
                {
                  NS_LOG_LOGIC ("Notifying MAC of idle channel");
                  m_lorazigbeeMacStateCallback (CHANNEL_IDLE);
                }
            }
        }
      else
        {
          if (IsSlottedCsmaCa ())
            {
              m_CW = 2;
            }
          m_BE = std::min (static_cast<uint16_t> (m_BE + 1), static_cast<uint16_t> (m_macMaxBE));
          m_NB++;
          if (m_NB > m_macMaxCSMABackoffs)
            {
              // no channel found so cannot send pkt
              NS_LOG_DEBUG ("Channel access failure");
              if (!m_lorazigbeeMacStateCallback.IsNull ())
                {
                  NS_LOG_LOGIC ("Notifying MAC of Channel access failure");
                  m_lorazigbeeMacStateCallback (CHANNEL_ACCESS_FAILURE);
                }
              return;
            }
          else
            {
              NS_LOG_DEBUG ("Perform another backoff; m_NB = " << static_cast<uint16_t> (m_NB));
              m_randomBackoffEvent = Simulator::ScheduleNow (&LoraZigBeeCsmaCa::RandomBackoffDelay, this); //Perform another backoff (step 2)
            }
        }
    }
}

void
LoraZigBeeCsmaCa::SetLoraZigBeeMacStateCallback (LoraZigBeeMacStateCallback c)
{
  NS_LOG_FUNCTION (this);
  m_lorazigbeeMacStateCallback = c;
}

int64_t
LoraZigBeeCsmaCa::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this);
  m_random->SetStream (stream);
  return 1;
}

uint8_t
LoraZigBeeCsmaCa::GetNB (void)
{
  return m_NB;
}

} //namespace ns3
