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

#ifndef __FWDATA_MT_OBJECTREADTOWRITELOCK_HPP__
#define __FWDATA_MT_OBJECTREADTOWRITELOCK_HPP__

#include "fwData/Object.hpp"
#include "fwData/config.hpp"

#include <fwCore/mt/types.hpp>

namespace fwData
{
namespace mt
{

/**
 * @brief  A helper to lock object on upgradable mode.
 */
class FWDATA_CLASS_API ObjectReadToWriteLock
{

public:

    /**
     * @brief Constructor : owns an upgradable lock on object mutex.
     * If adopt_lock==false : the mutex is not locked (call lock() to lock mutex)
     */
    FWDATA_API ObjectReadToWriteLock( ::fwData::Object::sptr obj, bool lock = true );

    /// Destructor. Does nothing
    FWDATA_API ~ObjectReadToWriteLock();

    /// Adds upgradable lock on object mutex
    FWDATA_API void lock();

    /// Releases upgradable lock on object mutex
    FWDATA_API void unlock();

    /// Upgrade lock to exclusive
    FWDATA_API void upgrade();

    /// Downgrade exclusive lock to upgradable lock
    FWDATA_API void downgrade();

private:

    /// lock on object mutex
    ::fwCore::mt::ReadToWriteLock m_lock;
    SPTR(::fwCore::mt::UpgradeToWriteLock) m_upgradedLock;
};

} // mt
} // fwData

#endif // __FWDATA_MT_OBJECTREADTOWRITELOCK_HPP__
