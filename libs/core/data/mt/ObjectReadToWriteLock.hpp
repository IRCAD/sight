/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/Object.hpp"

#include <core/mt/types.hpp>

namespace sight::data
{
namespace mt
{

/**
 * @brief  A helper to lock object on upgradable mode.
 * @deprecated use locked_ptr instead, or rely on service::IService::getLockedXXX() to directly receive a
 * locked_ptr
 */
class DATA_CLASS_API ObjectReadToWriteLock
{

public:

    /**
     * @brief Constructor : owns an upgradable lock on object mutex.
     * @deprecated use locked_ptr instead, or rely on service::IService::getLockedXXX() to directly receive a
     * locked_ptr
     * If adopt_lock==false : the mutex is not locked (call lock() to lock mutex)
     */
    [[deprecated("it will be removed in sight 21.0, use locked_ptr")]]
    DATA_API ObjectReadToWriteLock( data::Object::sptr obj, bool lock = true );

    /// Destructor. Does nothing
    DATA_API ~ObjectReadToWriteLock();

    /// Adds upgradable lock on object mutex
    DATA_API void lock();

    /// Releases upgradable lock on object mutex
    DATA_API void unlock();

    /// Upgrade lock to exclusive
    DATA_API void upgrade();

    /// Downgrade exclusive lock to upgradable lock
    DATA_API void downgrade();

private:

    /// lock on object mutex
    core::mt::ReadToWriteLock m_lock;
    SPTR(core::mt::UpgradeToWriteLock) m_upgradedLock;
};

} // mt
} // fwData
