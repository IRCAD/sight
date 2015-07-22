/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGuiQt/dialog/ProgressDialog.hpp>
#include <fwData/Composite.hpp>



#include "ioDicomExt/common/controller/SProgressBarController.hpp"

namespace ioDicomExt
{

namespace common
{

namespace controller
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioDicomExt::common::controller::SProgressBarController,
                         ::fwData::Composite );

const ::fwCom::Slots::SlotKeyType SProgressBarController::s_PROGRESS_SLOT = "progressBar";

//------------------------------------------------------------------------------

SProgressBarController::SProgressBarController() throw()
{
    m_slotProgressBar = ::fwCom::newSlot(&SProgressBarController::progressBar, this);
    ::fwCom::HasSlots::m_slots(s_PROGRESS_SLOT, m_slotProgressBar);

#ifdef COM_LOG
    m_slotProgressBar->setID( s_PROGRESS_SLOT );
#endif

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}
//------------------------------------------------------------------------------

SProgressBarController::~SProgressBarController() throw()
{
}

//------------------------------------------------------------------------------

void SProgressBarController::info(std::ostream &_sstream )
{
    _sstream << "SProgressBarController::info";
}

//------------------------------------------------------------------------------

void SProgressBarController::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SProgressBarController::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------


void SProgressBarController::progressBar(::ioDicomExt::common::data::ProgressMsg::sptr progressEvent)
{
    std::string id = progressEvent->getBarID();
    if(progressEvent->hasEvent(::ioDicomExt::common::data::ProgressMsg::s_STOP_PROGRESS_BAR))
    {
        m_progressDialogs.erase(id);
    }
    else
    {
        if(progressEvent->hasEvent(::ioDicomExt::common::data::ProgressMsg::s_START_PROGRESS_BAR))
        {
            m_progressDialogs[id] = ::fwGuiQt::dialog::ProgressDialog::New();

        }
        if(m_progressDialogs.find(id) != m_progressDialogs.end())
        {
            (*m_progressDialogs[id])(progressEvent->getPercentage(),progressEvent->getMessage());
            m_progressDialogs[id]->setMessage(progressEvent->getMessage());
        }
        else
        {
            SLM_WARN("Trying to update a progress bar which is not started !");
        }
    }

}

//------------------------------------------------------------------------------

} // namespace controller
} // namespace common
} // namespace ioDicomExt
