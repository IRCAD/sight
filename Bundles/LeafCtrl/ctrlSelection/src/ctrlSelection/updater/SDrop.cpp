/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/updater/SDrop.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/UUID.hpp>


namespace ctrlSelection
{

namespace updater
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECT_SLOT = "addObject";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::SDrop, ::fwData::Composite );

//-----------------------------------------------------------------------------

SDrop::SDrop() noexcept
{
    newSlot(s_ADD_OBJECT_SLOT, &SDrop::addObject, this);
}

//-----------------------------------------------------------------------------

SDrop::~SDrop() noexcept
{
}

//-----------------------------------------------------------------------------

void SDrop::addObject( std::string uid )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::Object::sptr object = ::fwData::Object::dynamicCast(::fwTools::UUID::get(uid));
    if(object)
    {
        ::fwDataTools::helper::Composite helper( this->getObject< ::fwData::Composite >() );
        helper.clear();
        if(object->isA("::fwData::Image"))
        {
            helper.add("image", object);
        }
        else if(object->isA("::fwData::Mesh"))
        {
            helper.add("mesh", object);
        }
        else if(object->isA("::fwData::Reconstruction"))
        {
            helper.add("reconstruction", object);
        }
        else if(object->isA("::fwData::Resection"))
        {
            helper.add("resection", object);
        }
        else if(object->isA("::fwData::ResectionDB"))
        {
            helper.add("resectionDB", object);
        }
        else if(object->isA("::fwData::Plane"))
        {
            helper.add("plane", object);
        }
        else if(object->isA("::fwData::PlaneList"))
        {
            helper.add("planeList", object);
        }
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SDrop::starting()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDrop::stopping()
{
}

//-----------------------------------------------------------------------------

void SDrop::configuring()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDrop::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void SDrop::updating()
{
}

//-----------------------------------------------------------------------------

void SDrop::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
