/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/mt/ObjectWriteLock.hpp"

#include <boost/thread/locks.hpp>

namespace fwData
{
namespace mt
{

//-----------------------------------------------------------------------------

ObjectWriteLock::ObjectWriteLock(::fwData::Object::sptr obj, bool adopt_lock)
{
    if (adopt_lock)
    {
        m_lock = ::fwCore::mt::WriteLock(obj->getMutex());
    }
    else
    {
        m_lock = ::fwCore::mt::WriteLock(obj->getMutex(), ::boost::defer_lock_t());
    }
}

//-----------------------------------------------------------------------------

ObjectWriteLock::~ObjectWriteLock()
{
}

//-----------------------------------------------------------------------------

void ObjectWriteLock::lock()
{
    m_lock.lock();
}

//-----------------------------------------------------------------------------

void ObjectWriteLock::unlock()
{
    m_lock.unlock();
}

//-----------------------------------------------------------------------------

} // mt
} // fwData
