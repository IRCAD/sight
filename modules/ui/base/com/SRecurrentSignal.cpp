/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "SRecurrentSignal.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/runtime/ConfigurationElement.hpp>

#include <data/Object.hpp>

#include <service/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>

namespace sight::module::ui::base::com
{

const core::com::Signals::SignalKeyType SRecurrentSignal::s_REPEATED_SIGNAL       = "repeated";
const core::com::Signals::SignalKeyType SRecurrentSignal::s_REPEAT_STOPPED_SIGNAL = "repeatStopped";

// ----------------------------------------------------------------------------

SRecurrentSignal::SRecurrentSignal() noexcept :

    m_sigRepeated(RepeatedSignalType::New())
{
    m_sigRepeated = newSignal<RepeatedSignalType>(s_REPEATED_SIGNAL);

    newSignal<RepeatStoppedSignalType>(s_REPEAT_STOPPED_SIGNAL);
}

// ----------------------------------------------------------------------------

void SRecurrentSignal::configuring()
{
    core::runtime::ConfigurationElement::sptr timeStepConfig = m_configuration->findConfigurationElement("timeStep");
    SIGHT_WARN_IF("Missing \"timeStep\" tag.", !timeStepConfig);
    if(timeStepConfig)
    {
        m_timeStep = boost::lexical_cast<unsigned int>(timeStepConfig->getValue());
    }
}

// ----------------------------------------------------------------------------

void SRecurrentSignal::starting()
{
    m_timer = m_associatedWorker->createTimer();
    core::thread::Timer::TimeDurationType duration = std::chrono::milliseconds(m_timeStep);
    m_timer->setFunction([this](auto&& ...){updating();});
    m_timer->setDuration(duration);
    m_timer->start();
}

// ----------------------------------------------------------------------------

void SRecurrentSignal::stopping()
{
    m_timer->stop();
    auto sig = this->signal<RepeatStoppedSignalType>(s_REPEAT_STOPPED_SIGNAL);
    sig->emit();
}

// ----------------------------------------------------------------------------

void SRecurrentSignal::updating()
{
    core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
    m_sigRepeated->asyncEmit(timestamp);
}

// ----------------------------------------------------------------------------

} // namespace sight::module::ui::base::com
