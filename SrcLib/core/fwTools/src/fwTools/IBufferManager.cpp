/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/IBufferManager.hpp"

namespace fwTools
{

IBufferManager::sptr IBufferManager::s_currentManager = IBufferManager::New();

::fwCore::mt::ReadWriteMutex IBufferManager::s_mutex;

//-----------------------------------------------------------------------------

IBufferManager::sptr IBufferManager::getCurrent()
{
    ::fwCore::mt::ReadLock lock(s_mutex);
    return s_currentManager;
}

//-----------------------------------------------------------------------------

void IBufferManager::setCurrent( IBufferManager::sptr currentManager )
{
    ::fwCore::mt::WriteLock lock(s_mutex);
    s_currentManager = currentManager;
}

} //namespace fwTools
