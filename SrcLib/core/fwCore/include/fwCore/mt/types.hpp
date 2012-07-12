/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
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
typedef ::boost::mutex::scoped_lock ScopedLock;

typedef ::boost::recursive_mutex RecursiveMutex;
typedef ::boost::recursive_mutex::scoped_lock RecursiveScopedLock;

typedef ::boost::shared_mutex SharedMutex;
typedef ::boost::shared_lock< SharedMutex > ReadLock;
typedef ::boost::unique_lock< SharedMutex > WriteLock;
typedef ::boost::upgrade_lock< SharedMutex > ReadToWriteLock;
typedef ::boost::upgrade_to_unique_lock< SharedMutex > UpgradeToWriteLock;

} //namespace mt
} //namespace fwCore

#endif /* __FWCORE_MT_TYPES_HPP__ */
