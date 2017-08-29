/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

