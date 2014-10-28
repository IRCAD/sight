/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/thread/locks.hpp>
#include <boost/make_shared.hpp>

#include "fwData/mt/ObjectReadToWriteLock.hpp"

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
{}

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
    m_upgradedLock = ::boost::make_shared< ::fwCore::mt::UpgradeToWriteLock >(::boost::ref(m_lock));
}

//-----------------------------------------------------------------------------

void ObjectReadToWriteLock::downgrade()
{
    m_upgradedLock.reset();
}

//-----------------------------------------------------------------------------

} // mt
} // fwData
