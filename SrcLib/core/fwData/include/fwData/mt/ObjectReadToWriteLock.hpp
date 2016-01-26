/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
