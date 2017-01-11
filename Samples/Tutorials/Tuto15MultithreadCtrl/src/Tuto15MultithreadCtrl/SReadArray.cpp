/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto15MultithreadCtrl/SReadArray.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwServices/macros.hpp>


fwServicesRegisterMacro( ::fwServices::IController, ::Tuto15MultithreadCtrl::SReadArray, ::fwData::Array );

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
}

void SReadArray::stopping() throw( ::fwTools::Failed )
{
}

void SReadArray::updating() throw( ::fwTools::Failed )
{
    ::fwData::Array::sptr array = this->getObject< ::fwData::Array >();
    ::fwData::mt::ObjectWriteLock writeLock(array);
    SLM_ASSERT("No array.", array);

    // Initialize the array size and type
    const int arraySize = 10;
    ::fwData::Array::SizeType size(1, arraySize);
    array->resize("uint32", size, 1, true);

    // Fill the array values
    ::fwDataTools::helper::Array arrayHelper(array);
    unsigned int* buffer = static_cast< unsigned int* >( arrayHelper.getBuffer() );
    for (unsigned int i = 0; i < arraySize; i++)
    {
        buffer[i] = i;
    }

    // Notify that the array is modified
    ::fwData::Object::ModifiedSignalType::sptr sig
        = array->signal< ::fwData::Object::ModifiedSignalType>( ::fwData::Object::s_MODIFIED_SIG );
    sig->asyncEmit();
}

void SReadArray::configuring() throw( ::fwTools::Failed )
{

}

} // namespace Tuto15MultithreadCtrl
