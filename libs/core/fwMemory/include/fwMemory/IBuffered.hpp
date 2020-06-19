/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#pragma once

#include "fwMemory/BufferObject.hpp"
#include "fwMemory/config.hpp"

namespace fwData
{
namespace mt
{
template< class DATATYPE >
class locked_ptr;
}
}

namespace fwMemory
{

/**
 * @brief Defines the interface to implement to allow locking of data that use a BufferObject
 */
class FWMEMORY_CLASS_API IBuffered
{
protected:

    template< class DATATYPE >
    friend class ::fwData::mt::locked_ptr;

    /**
     * @brief Must allocate a ::fwMemory::BufferObject::Lock and store it into the vector parameter
     *
     * This allow locking of complex object with several BufferObject
     */
    FWMEMORY_API virtual void lockBuffer(std::vector< ::fwMemory::BufferObject::Lock >& locks) const = 0;
};

}
