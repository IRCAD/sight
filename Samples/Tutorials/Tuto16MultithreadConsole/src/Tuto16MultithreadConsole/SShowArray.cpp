/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto16MultithreadConsole/SShowArray.hpp"

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwDataTools/helper/ArrayGetter.hpp>

#include <fwServices/macros.hpp>

#include <sstream>

fwServicesRegisterMacro( ::fwServices::IController, ::Tuto16MultithreadConsole::SShowArray, ::fwData::Array );

namespace Tuto16MultithreadConsole
{

//------------------------------------------------------------------------------

SShowArray::SShowArray() noexcept
{
}

//------------------------------------------------------------------------------

SShowArray::~SShowArray() noexcept
{
}

//------------------------------------------------------------------------------

void SShowArray::starting()
{
}

//------------------------------------------------------------------------------

void SShowArray::stopping()
{
}

//------------------------------------------------------------------------------

void SShowArray::updating()
{
    ::fwData::Array::csptr array = this->getInput< ::fwData::Array >("array");
    ::fwData::mt::ObjectReadLock readLock(array);
    SLM_ASSERT("No array.", array);

    ::fwDataTools::helper::ArrayGetter arrayHelper(array);
    const unsigned int* buffer = static_cast< const unsigned int* >( arrayHelper.getBuffer() );

    std::stringstream str;
    std::ostream_iterator<unsigned int> coutIter(str, ", ");
    std::copy(buffer, buffer+10, coutIter );
    SLM_INFO("Buffer : " + str.str());
}

//------------------------------------------------------------------------------

void SShowArray::configuring()
{
}

//------------------------------------------------------------------------------

} // namespace Tuto16MultithreadConsole
