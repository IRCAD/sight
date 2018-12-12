/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "fwData/mt/ObjectReadLock.hpp"

#include <boost/thread/locks.hpp>

namespace fwData
{
namespace mt
{

//-----------------------------------------------------------------------------

ObjectReadLock::ObjectReadLock(::fwData::Object::csptr obj, bool lock)
{
    if (lock)
    {
        m_lock = ::fwCore::mt::ReadLock(obj->getMutex());
    }
    else
    {
        m_lock = ::fwCore::mt::ReadLock(obj->getMutex(), ::boost::defer_lock_t());
    }
}

//-----------------------------------------------------------------------------

ObjectReadLock::~ObjectReadLock()
{
}

//-----------------------------------------------------------------------------

void ObjectReadLock::lock()
{
    m_lock.lock();
}

//-----------------------------------------------------------------------------

void ObjectReadLock::unlock()
{
    m_lock.unlock();
}

//-----------------------------------------------------------------------------

} // mt
} // fwData
