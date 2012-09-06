/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Acquisition.hpp>

#include <fwComEd/helper/Composite.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/updater/SDrop.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::SDrop, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SDrop::SDrop() throw()
{
    this->addNewHandledEvent("DROPPED_UUID");
}

//-----------------------------------------------------------------------------

SDrop::~SDrop() throw()
{}

//-----------------------------------------------------------------------------

void SDrop::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwData::Object::csptr msgObject = _msg->getDataInfo("DROPPED_UUID");

    ::fwData::String::csptr id = ::fwData::String::dynamicConstCast(msgObject);

    ::fwData::Object::sptr object = ::fwData::Object::dynamicCast(::fwTools::UUID::get(id->getValue()));
    if(object)
    {

        ::fwComEd::helper::Composite helper( this->getObject< ::fwData::Composite >() );
        helper.clear();
        if(object->isA("::fwData::Acquisition"))
        {
            ::fwData::Acquisition::sptr acq = ::fwData::Acquisition::dynamicCast(object);
            helper.add("acquisition", object);
            helper.add("image", acq->getImage());
        }
        else if(object->isA("::fwData::Image"))
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
        helper.notify(this->getSptr());
    }

}


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void SDrop::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDrop::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SDrop::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDrop::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SDrop::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SDrop::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
