/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto15MultithreadCtrl/SReadArray.hpp"

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwComEd/helper/Array.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwCom/Signal.hxx>

fwServicesRegisterMacro( ::fwServices::IService , ::Tuto15MultithreadCtrl::SReadArray , ::fwData::Array ) ;

namespace Tuto15MultithreadCtrl
{

SReadArray::SReadArray() throw()
{
}

SReadArray::~SReadArray() throw()
{
}

void SReadArray::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void SReadArray::stopping() throw( ::fwTools::Failed )
{

}

void SReadArray::updating() throw( ::fwTools::Failed )
{
    ::fwData::Array::sptr array = this->getObject< ::fwData::Array >();
    ::fwData::mt::ObjectWriteLock writeLock(array);
    SLM_ASSERT("No array.", array);

    const int arraySize = 10;

    ::fwData::Array::SizeType size(1, arraySize);

    array->resize("uint32", size, 1, true);

    ::fwComEd::helper::Array arrayHelper(array);

    unsigned int *buffer = static_cast< unsigned int* >( arrayHelper.getBuffer() );

    for (int i = 0 ; i < arraySize; i++)
    {
        buffer[i] = i;
    }

    ::fwData::Object::ObjectModifiedSignalType::sptr sig
        = array->signal< ::fwData::Object::ObjectModifiedSignalType>( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

    ::fwServices::ObjectMsg::sptr msg = ::fwServices::ObjectMsg::New();
    msg->addEvent("MODIFIED_EVENT");
    sig->asyncEmit(msg);
}

void SReadArray::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{

}

void SReadArray::swapping( ) throw( ::fwTools::Failed )
{

}

void SReadArray::configuring() throw( ::fwTools::Failed )
{

}

} // namespace Tuto15MultithreadCtrl
