/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWDATA_MT_OBJECTWRITELOCK_HPP__
#define __FWDATA_MT_OBJECTWRITELOCK_HPP__

#include "fwData/Object.hpp"
#include "fwData/config.hpp"

#include <fwCore/mt/types.hpp>

namespace fwData
{
namespace mt
{

/**
 * @brief  A helper to lock object on exclusive mode.
 */
class FWDATA_CLASS_API ObjectWriteLock
{

public:

    /**
     * @brief Constructor : owns an exclusive lock on object mutex.
     * If adopt_lock==false : the mutex is not locked (call lock() to lock mutex)
     */
    FWDATA_API ObjectWriteLock( ::fwData::Object::sptr obj, bool adopt_lock = true );

    /// Destructor. Does nothing
    FWDATA_API ~ObjectWriteLock();

    /// Adds exclusive lock on object mutex
    FWDATA_API void lock();

    /// Releases lock on object mutex
    FWDATA_API void unlock();

private:

    /// lock on object mutex
    ::fwCore::mt::WriteLock m_lock;
};

} // mt
} // fwData

#endif // __FWDATA_MT_OBJECTWRITELOCK_HPP__
