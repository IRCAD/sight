/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/thread/locks.hpp>

#include "fwData/mt/ObjectReadLock.hpp"

namespace fwData
{
namespace mt
{

//-----------------------------------------------------------------------------

ObjectReadLock::ObjectReadLock(::fwData::Object::sptr obj, bool lock)
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
