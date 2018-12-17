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

#include "fwData/mt/ObjectReadToWriteLock.hpp"

#include <boost/thread/locks.hpp>

namespace fwData
{
namespace mt
{

//-----------------------------------------------------------------------------

ObjectReadToWriteLock::ObjectReadToWriteLock(::fwData::Object::sptr obj, bool lock)
{
    if (lock)
    {
        m_lock = ::fwCore::mt::ReadToWriteLock(obj->getMutex());
    }
    else
    {
        m_lock = ::fwCore::mt::ReadToWriteLock(obj->getMutex(), ::boost::defer_lock_t());
    }
}

//-----------------------------------------------------------------------------

ObjectReadToWriteLock::~ObjectReadToWriteLock()
{
}

//-----------------------------------------------------------------------------

void ObjectReadToWriteLock::lock()
{
    m_lock.lock();
}

//-----------------------------------------------------------------------------

void ObjectReadToWriteLock::unlock()
{
    m_lock.unlock();
}

//-----------------------------------------------------------------------------

void ObjectReadToWriteLock::upgrade()
{
    m_upgradedLock = std::make_shared< ::fwCore::mt::UpgradeToWriteLock >(std::ref(m_lock));
}

//-----------------------------------------------------------------------------

void ObjectReadToWriteLock::downgrade()
{
    m_upgradedLock.reset();
}

//-----------------------------------------------------------------------------

} // mt
} // fwData
