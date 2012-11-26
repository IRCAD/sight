/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/helper/Array.hpp>

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include "Tuto15MultithreadCtrl/SIncrementArray.hpp"

fwServicesRegisterMacro( ::fwServices::IService , ::Tuto15MultithreadCtrl::SIncrementArray , ::fwData::Array ) ;

namespace Tuto15MultithreadCtrl
{

SIncrementArray::SIncrementArray() throw()
{
}

SIncrementArray::~SIncrementArray() throw()
{
}

void SIncrementArray::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void SIncrementArray::stopping() throw( ::fwTools::Failed )
{

}

void SIncrementArray::updating() throw( ::fwTools::Failed )
{
    ::fwData::Array::sptr array = this->getObject< ::fwData::Array >();
    ::fwData::mt::ObjectWriteLock writeLock(array);

    SLM_ASSERT("No array.", array);

    const int arraySize = 10;

    ::fwComEd::helper::Array arrayHelper(array);

    unsigned int *buffer = static_cast< unsigned int* >( arrayHelper.getBuffer() );

    for (int i = 0 ; i < arraySize; i++)
    {
        ++buffer[i];
    }

    ::fwData::Object::ObjectModifiedSignalType::sptr sig
        = array->signal< ::fwData::Object::ObjectModifiedSignalType>( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

    ::fwServices::ObjectMsg::csptr msg;// = ::fwServices::ObjectMsg::sptr( (::fwServices::ObjectMsg*) NULL );
    sig->emit(msg);


}

void SIncrementArray::configuring() throw( ::fwTools::Failed )
{

}

void SIncrementArray::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{

}

void SIncrementArray::swapping( ) throw( ::fwTools::Failed )
{

}

} // namespace Tuto15MultithreadCtrl
