/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/Base.hpp>

#include <fwData/Image.hpp>

#include <fwComEd/ImageMsg.hpp>


#include "ctrlSelection/wrapper/ImageWrapperSrv.hpp"


fwServicesRegisterMacro( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::ImageWrapperSrv, ::fwData::Image );

namespace ctrlSelection
{

namespace wrapper
{

//-----------------------------------------------------------------------------

ImageWrapperSrv::ImageWrapperSrv() throw()
{
    //TODO addNewHandledEvent( ::fwServices:: ObjectMsg::NEW_OBJECT );
    //handlingEventOff ::fwServices::ObjectMsg::UPDATED_OBJECT );
    //TODO addNewHandledEvent( ::fwServices:: ObjectMsg::DELETE_OBJECT );
}

//-----------------------------------------------------------------------------

ImageWrapperSrv::~ImageWrapperSrv() throw()
{
}

//-----------------------------------------------------------------------------

void ImageWrapperSrv::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    if ( message->hasEvent( ::fwServices:: ObjectMsg::UPDATED_OBJECT ) )
    {
        assert( message->getDataInfo( ::fwServices:: ObjectMsg::UPDATED_OBJECT   ) == this->getObject() );
        fwComEd::ImageMsg::sptr msg = fwComEd::ImageMsg::New();
        msg->addEvent( fwComEd::ImageMsg::NEW_IMAGE, this->getObject() );
        msg->addEvent( fwComEd::ImageMsg::BUFFER, this->getObject() );
        msg->setSource(this->getSptr());
        msg->setSubject( this->getObject());
        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
        sig = this->getObject()->signal< ::fwData::Object::ObjectModifiedSignalType >(
            ::fwData::Object::s_OBJECT_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
            sig->asyncEmit( msg);
        }
    }
    //TODO other event
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
