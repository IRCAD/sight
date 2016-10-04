/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlPointSelection/SUpdateNegato.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwData/Integer.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>


namespace ctrlPointSelection
{

fwServicesRegisterMacro(::fwServices::IController, ::ctrlPointSelection::SUpdateNegato, ::fwData::Image);

const ::fwCom::Slots::SlotKeyType SUpdateNegato::s_SELECT_NEW_POINT_SLOT = "selectNewPoint";

SUpdateNegato::SUpdateNegato() throw()
{
    newSlot(s_SELECT_NEW_POINT_SLOT, &SUpdateNegato::updateSlices, this);
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
        auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

} // ctrlPointSelection

