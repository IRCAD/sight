/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCom/SSignalGate.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Object.hpp>

#include <future>
#include <regex>

namespace ctrlCom
{

// Public signal
const ::fwCom::Signals::SignalKeyType SSignalGate::s_ALL_RECEIVED_SIG = "allReceived";

// Private slot
static const ::fwCom::Slots::SlotKeyType s_RECEIVE_SLOT = "receive";

//-----------------------------------------------------------------------------

SSignalGate::SSignalGate()
{
    newSignal< AllReceivedSignalType>(s_ALL_RECEIVED_SIG);

}

//-----------------------------------------------------------------------------

SSignalGate::~SSignalGate()
{
}

//-----------------------------------------------------------------------------

void SSignalGate::configuring()
{
}

//------------------------------------------------------------------------------

void SSignalGate::starting()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    const std::regex re("(.*)/(.*)");
    std::smatch match;

    auto signalsCfg = config.equal_range("signal");
    for (auto itCfg = signalsCfg.first; itCfg != signalsCfg.second; ++itCfg)
    {
        const std::string& signal = itCfg->second.get_value<std::string>();
        if( std::regex_match(signal, match, re) )
        {
            SLM_ASSERT("Wrong value for attribute src: " + signal, match.size() >= 3);

            std::string uid, signalKey;
            uid.assign(match[1].first, match[1].second);
            signalKey.assign(match[2].first, match[2].second);

            if (::fwTools::fwID::exist(uid))
            {
                ::fwTools::Object::sptr obj             = ::fwTools::fwID::getObject(uid);
                ::fwCom::HasSignals::sptr signalsHolder = std::dynamic_pointer_cast< ::fwCom::HasSignals >(obj);
                SLM_ASSERT("Object with id " << uid << " is not a HasSlots", signalsHolder);

                const size_t index = m_flags.size();
                m_flags.push_back(false);

                // Create a slot to our callback with a bound index to identify it
                std::function<void()> task(std::bind(&SSignalGate::received, this, index));
                auto slot = ::fwCom::newSlot(task);
                slot->setWorker(m_associatedWorker);

                // Connect the configured signal to this slot
                auto sig = signalsHolder->signal(signalKey);
                SLM_ASSERT("Object with id " + uid + " does not have a signalKey '" + signalKey + "'", sig);

                sig->connect(slot);

                m_waitingSlots.push_back(slot);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SSignalGate::stopping()
{
    m_waitingSlots.clear();
    m_flags.clear();
}

//-----------------------------------------------------------------------------

void SSignalGate::updating()
{
}

//------------------------------------------------------------------------------

void SSignalGate::received(size_t _index)
{
    OSLM_DEBUG("'" << this->getID() << "' received a signal at position : " << _index);
    OSLM_ASSERT("Could not find a signal at index " <<_index, _index < m_flags.size() );

    m_flags[_index] = true;

    bool allReceived = true;
    for(bool received : m_flags)
    {
        allReceived &= received;
    }

    if(allReceived)
    {
        OSLM_DEBUG("'" << this->getID() << "' received all signals, sending 'allReceived' now.");
        auto sig = this->signal<AllReceivedSignalType>(s_ALL_RECEIVED_SIG);
        sig->asyncEmit();

        // Reset all flags
        std::fill(m_flags.begin(), m_flags.end(), false);
    }
}

//-----------------------------------------------------------------------------

} // namespace ctrlCom
