/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto16MultithreadConsole/SReadArray.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwServices::IController, ::Tuto16MultithreadConsole::SReadArray, ::fwData::Array );

namespace Tuto16MultithreadConsole
{

//------------------------------------------------------------------------------

SReadArray::SReadArray() noexcept
{
}

//------------------------------------------------------------------------------

SReadArray::~SReadArray() noexcept
{
}

//------------------------------------------------------------------------------

void SReadArray::starting()
{
}

//------------------------------------------------------------------------------

void SReadArray::stopping()
{
}

//------------------------------------------------------------------------------

void SReadArray::updating()
{
    ::fwData::Array::sptr array = this->getInOut< ::fwData::Array >("array");
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

//------------------------------------------------------------------------------

void SReadArray::configuring()
{

}

//------------------------------------------------------------------------------

} // namespace Tuto16MultithreadConsole
