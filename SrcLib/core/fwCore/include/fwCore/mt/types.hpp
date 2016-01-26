/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_MT_TYPES_HPP__
#define __FWCORE_MT_TYPES_HPP__

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace fwCore
{
namespace mt
{

typedef ::boost::mutex Mutex;
typedef ::boost::unique_lock< Mutex > ScopedLock;

typedef ::boost::recursive_mutex RecursiveMutex;
typedef ::boost::unique_lock< RecursiveMutex > RecursiveScopedLock;

/// Defines a single writer, multiple readers mutex.
typedef ::boost::shared_mutex ReadWriteMutex;
/**
 * @brief Defines a lock of read type for read/write mutex.
 * @note Multiple read lock can be done.
 */
typedef ::boost::shared_lock< ReadWriteMutex > ReadLock;
/**
 * @brief Defines a lock of write type for read/write mutex.
 * @note Only one write lock can be done at once.
 */
typedef ::boost::unique_lock< ReadWriteMutex > WriteLock;
/**
 * @brief Defines an upgradable lock type for read/write mutex.
 * @note Only one upgradable lock can be done at once but there may be multiple read lock.
 */
typedef ::boost::upgrade_lock< ReadWriteMutex > ReadToWriteLock;
/**
 * @brief Defines a write lock upgraded from ReadToWriteLock.
 * @note Only one upgradable lock can be done at once but there may be multiple read lock.
 */
typedef ::boost::upgrade_to_unique_lock< ReadWriteMutex > UpgradeToWriteLock;

} //namespace mt
} //namespace fwCore

#endif /* __FWCORE_MT_TYPES_HPP__ */
