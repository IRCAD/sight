/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SSignal.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/Extension.hpp>
#include <fwRuntime/helper.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

namespace gui
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SSignal, ::fwData::Object );


static const ::fwCom::Signals::SignalKeyType TRIGERRED_SIG   = "triggered";
static const ::fwCom::Signals::SignalKeyType CANCELLED_SIG   = "cancelled";
static const ::fwCom::Signals::SignalKeyType SET_OBJECT_SLOT = "setObjects";

//-----------------------------------------------------------------------------

SSignal::SSignal() noexcept :
    m_sigTriggered(TrigerredSignalType::New()),
    m_sigCancelled(TrigerredSignalType::New()),
    m_slotSetObjects(SSignal::SetObjectsSlotType::New(&SSignal::setObjects, this))
{
    ::fwCom::HasSignals::m_signals
        ( TRIGERRED_SIG, m_sigTriggered )
        ( CANCELLED_SIG, m_sigCancelled );

    ::fwCom::HasSlots::m_slots
        ( SET_OBJECT_SLOT, m_slotSetObjects );



    this->setWorker( m_associatedWorker );
}

//-----------------------------------------------------------------------------

SSignal::~SSignal() noexcept
{
}

//-----------------------------------------------------------------------------

void SSignal::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SSignal::stopping()
{
    SLM_TRACE_FUNC();

    m_objects.clear();

    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SSignal::info(std::ostream &_sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SSignal::updating()
{
    SLM_TRACE_FUNC();
    if (this->confirmAction())
    {
        m_sigTriggered->asyncEmit(m_objects);
    }
    else
    {
        m_sigCancelled->asyncEmit(m_objects);
    }

}

//-----------------------------------------------------------------------------

void SSignal::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

} // namespace action
} // namespace gui

