/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SSignal.hpp"

#include <core/com/Signal.hxx>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::ui::base
{

namespace com
{

//-----------------------------------------------------------------------------

static const core::com::Signals::SignalKeyType s_TRIGGERED_SIG = "triggered";
static const core::com::Signals::SignalKeyType s_CANCELLED_SIG = "cancelled";

//-----------------------------------------------------------------------------

SSignal::SSignal() noexcept :
    m_sigTriggered(TriggeredSignalType::New()),
    m_sigCancelled(TriggeredSignalType::New())
{
    m_sigTriggered = newSignal<TriggeredSignalType>(s_TRIGGERED_SIG);
    m_sigCancelled = newSignal<TriggeredSignalType>(s_CANCELLED_SIG);
}

//-----------------------------------------------------------------------------

SSignal::~SSignal() noexcept
{
}

//-----------------------------------------------------------------------------

void SSignal::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SSignal::starting()
{
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SSignal::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SSignal::info(std::ostream& _sstream)
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SSignal::updating()
{
    if(this->confirmAction())
    {
        m_sigTriggered->asyncEmit(this->getIsActive());
    }
    else
    {
        m_sigCancelled->asyncEmit(this->getIsActive());
    }
}

//-----------------------------------------------------------------------------

} // namespace com

} // namespace sight::module::ui::base
