/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/IEditionService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwData/Integer.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwComEd/ImageMsg.hpp>

#include "ctrlPointSelection/SUpdateNegato.hpp"

namespace ctrlPointSelection
{

fwServicesRegisterMacro(::fwServices::IController, ::ctrlPointSelection::SUpdateNegato, ::fwData::Image);

const ::fwCom::Slots::SlotKeyType SUpdateNegato::s_SELECT_NEW_POINT_SLOT = "selectNewPoint";

SUpdateNegato::SUpdateNegato() throw()
{
    m_slotSelectNewPoint = ::fwCom::newSlot(&SUpdateNegato::updateSlices, this);
    ::fwCom::HasSlots::m_slots(s_SELECT_NEW_POINT_SLOT, m_slotSelectNewPoint);

    // Set default worker to new slot
    this->setWorker(::fwServices::registry::ActiveWorkers::getDefault()->
                    getWorker(::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER));
}

//-------------------------------------------------------------------------------------------------------------

SUpdateNegato::~SUpdateNegato() throw()
{
}

//-------------------------------------------------------------------------------------------------------------

void SUpdateNegato::configuring() throw(::fwTools::Failed)
{
}

//-------------------------------------------------------------------------------------------------------------

void SUpdateNegato::starting() throw(::fwTools::Failed)
{
}

//-------------------------------------------------------------------------------------------------------------

void SUpdateNegato::stopping() throw(::fwTools::Failed)
{
}

//-------------------------------------------------------------------------------------------------------------

void SUpdateNegato::updating() throw (::fwTools::Failed)
{
}

//-------------------------------------------------------------------------------------------------------------

void SUpdateNegato::receiving(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
}

//-------------------------------------------------------------------------------------------------------------

void SUpdateNegato::updateSlices(::fwData::Point::sptr selectedPoint)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image>();
    SLM_ASSERT("Failed to retrieve image", image);

    ::fwData::Integer::sptr paramA = ::fwData::Integer::New();
    ::fwData::Integer::sptr paramF = ::fwData::Integer::New();
    ::fwData::Integer::sptr paramS = ::fwData::Integer::New();

    // Sets the slices for the negato
    paramA->value() = static_cast<int>(
        (selectedPoint->getRefCoord()[2] - image->getOrigin()[2]) / image->getSpacing()[2] + 0.5);

    paramF->value() = static_cast<int>(
        (selectedPoint->getRefCoord()[1] - image->getOrigin()[1]) / image->getSpacing()[1] + 0.5);

    paramS->value() = static_cast<int>(
        (selectedPoint->getRefCoord()[0] - image->getOrigin()[0]) / image->getSpacing()[0] + 0.5);

    if(paramS->value() >= 0 && paramF->value() >= 0 && paramA->value() >= 0
       && image->getSize()[0] > paramS->value()
       && image->getSize()[1] > paramF->value()
       && image->getSize()[2] > paramA->value())
    {
        ::fwComEd::ImageMsg::sptr msg = ::fwComEd::ImageMsg::New();
        msg->setSliceIndex(paramA, paramF, paramS);
        ::fwServices::IEditionService::notify(this->getSptr(), image, msg);
    }
}

} // ctrlPointSelection

