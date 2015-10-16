/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IEditionService.hpp"
#include "fwServices/macros.hpp"
#include "fwServices/ObjectMsg.hpp"
#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/op/Get.hpp"

#include <fwCom/Signal.hxx>

namespace fwServices
{

//-----------------------------------------------------------------------------

void IEditionService::notify(
    ::fwServices::IService::sptr _pSource,
    ::fwData::Object::sptr _pSubject,
    ::fwServices::ObjectMsg::sptr _pMsg)
{

    _pMsg->setSource(_pSource);
    _pMsg->setSubject(_pSubject);

    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = _pSubject->signal< ::fwData::Object::ObjectModifiedSignalType >( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

    IService::ReceiveSlotType::sptr slot = _pSource->slot< IService::ReceiveSlotType >( IService::s_RECEIVE_SLOT );

    {
        ::fwCom::Connection::Blocker block(sig->getConnection(slot));
        sig->asyncEmit(_pMsg);
    }

}

//-----------------------------------------------------------------------------

void IEditionService::notify(
    ::fwServices::IService::sptr _pSource,
    ::fwData::Object::sptr _pSubject,
    ::fwServices::ObjectMsg::sptr _pMsg,
    bool _allowLoops )
{

    _pMsg->setSource(_pSource);
    _pMsg->setSubject(_pSubject);

    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = _pSubject->signal< ::fwData::Object::ObjectModifiedSignalType >( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

    if( _allowLoops )
    {
        sig->asyncEmit(_pMsg);
    }
    else
    {
        IService::ReceiveSlotType::sptr slot = _pSource->slot< IService::ReceiveSlotType >( IService::s_RECEIVE_SLOT );
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(slot));
            sig->asyncEmit(_pMsg);
        }
    }
}

//-----------------------------------------------------------------------------

IEditionService::IEditionService()
{
}

//-----------------------------------------------------------------------------

IEditionService::~IEditionService() throw()
{
}

//-----------------------------------------------------------------------------

}
