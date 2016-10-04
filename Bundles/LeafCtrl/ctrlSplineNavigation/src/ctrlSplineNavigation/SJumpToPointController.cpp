/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSplineNavigation/SJumpToPointController.hpp"

#include <fwCom/Slots.hxx>

// Services tools
#include <fwServices/op/Get.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/IService.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>


fwServicesRegisterMacro(
    ::fwServices::IController, ::ctrlSplineNavigation::SJumpToPointController, ::fwData::TransformationMatrix3D);


namespace ctrlSplineNavigation
{

static const std::string S_MATRIX_KEY = "matrix";

const ::fwCom::Slots::SlotKeyType SJumpToPointController::s_CHANGE_DIRECT_TARGET_SLOT = "changeDirectTarget";

//-----------------------------------------------------------------------------

SJumpToPointController::SJumpToPointController() throw ()
{
    newSlot(s_CHANGE_DIRECT_TARGET_SLOT, &SJumpToPointController::jumpToViewPoint, this);
}

//-----------------------------------------------------------------------------

SJumpToPointController::~SJumpToPointController() throw ()
{
}

//------------------------------------------------------------------------------

void SJumpToPointController::starting() throw (::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SJumpToPointController::stopping() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SJumpToPointController::configuring() throw (fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SJumpToPointController::updating() throw (::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SJumpToPointController::jumpToViewPoint(::fwData::TransformationMatrix3D::sptr matrix)
{
    OSLM_TRACE(" DisplayMatrixReceived " << *(matrix));

    // Get the current matrix
    ::fwData::TransformationMatrix3D::sptr currentMatrix = this->getInOut< ::fwData::TransformationMatrix3D >(
        S_MATRIX_KEY);

    // Modify this matrix
    for(int lt = 0; lt < 4; lt++)
    {
        for(int ct = 0; ct < 4; ct++)
        {
            currentMatrix->setCoefficient(lt, ct, matrix->getCoefficient(lt, ct));
        }
    }

    auto sig = currentMatrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

}   // namespace ctrlSplineNavigation

