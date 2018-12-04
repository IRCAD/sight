/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
