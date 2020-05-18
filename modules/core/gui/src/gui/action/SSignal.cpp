/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "gui/action/SSignal.hpp"

#include <fwCom/Signal.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace gui
{
namespace action
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SSignal );

static const ::fwCom::Signals::SignalKeyType s_TRIGGERED_SIG = "triggered";
static const ::fwCom::Signals::SignalKeyType s_CANCELLED_SIG = "cancelled";

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

void SSignal::info(std::ostream& _sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SSignal::updating()
{
    SLM_TRACE_FUNC();
    if (this->confirmAction())
    {
        m_sigTriggered->asyncEmit(this->getIsActive());
    }
    else
    {
        m_sigCancelled->asyncEmit(this->getIsActive());
    }

}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace gui

