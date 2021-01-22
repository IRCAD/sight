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

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

#include <core/mt/types.hpp>

namespace fwData
{
namespace mt
{

/**
 * @brief  A helper to lock object on exclusive mode.
 * @deprecated use locked_ptr instead, or rely on ::fwServices::IService::getLockedXXX() to directly receive a
 * locked_ptr
 */
class FWDATA_CLASS_API ObjectWriteLock
{

public:

    /**
     * @brief Constructor : owns an exclusive lock on object mutex.
     * @deprecated use locked_ptr instead, or rely on ::fwServices::IService::getLockedXXX() to directly receive a
     * locked_ptr
     * If adopt_lock==false : the mutex is not locked (call lock() to lock mutex)
     */
    [[deprecated("it will be removed in sight 21.0, use locked_ptr")]]
    FWDATA_API ObjectWriteLock( ::fwData::Object::sptr obj, bool adopt_lock = true );

    /// Destructor. Does nothing
    FWDATA_API ~ObjectWriteLock();

    /// Adds exclusive lock on object mutex
    FWDATA_API void lock();

    /// Releases lock on object mutex
    FWDATA_API void unlock();

private:

    /// lock on object mutex
    core::mt::WriteLock m_lock;
};

} // mt
} // fwData
