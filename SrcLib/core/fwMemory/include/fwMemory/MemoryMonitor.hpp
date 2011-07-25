/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_MEMORYMONITOR_HPP_
#define _FWMEMORY_MEMORYMONITOR_HPP_

#include <map>

#include <boost/cstdint.hpp>

#include <fwData/Object.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{

class IDumpPolicy ;

class MemoryMonitor
{
public:

    struct MemoryInfo
    {
        MemoryInfo()
        {
            virtualSizeInBytes = 0;
            currentSizeInBytes = 0;
            lastAccessTime = 0;
            dumpLockCount = 0;
        }

        ::boost::uint64_t virtualSizeInBytes;
        ::boost::uint64_t currentSizeInBytes;
        ::boost::uint64_t lastAccessTime;
        bool              dumpLockCount;
    };

    typedef std::map< ::fwTools::Object::wptr,  MemoryInfo > StatisticsContainer;



    FWMEMORY_API static const ::boost::shared_ptr< MemoryMonitor > getDefault() throw();

    FWMEMORY_API static void setPolicy( ::boost::shared_ptr< ::fwMemory::IDumpPolicy > _policy ) throw();

    FWMEMORY_API static const MemoryMonitor::StatisticsContainer & getStatistics() throw() ;



    FWMEMORY_API ~MemoryMonitor(); // not a plymorphic class

    FWMEMORY_API void registerObject( ::fwTools::Object::wptr ) throw();

    FWMEMORY_API void unregisterObject( ::fwTools::Object::wptr ) throw();

    /// remove dead weak pointer in m_statistics
    FWMEMORY_API void clean() throw();

    FWMEMORY_API void statusChanged( ::fwTools::Object::wptr, MemoryInfo &memInfo ) throw();

    FWMEMORY_API const MemoryMonitor::MemoryInfo getStatus( ::fwTools::Object::wptr obj ) const throw();

    FWMEMORY_API std::string log() throw();

    FWMEMORY_API ::boost::uint64_t totalUsedSizeInBytes();

    FWMEMORY_API  bool reserveMemory( ::boost::uint64_t nbBytes);

    FWMEMORY_API MemoryMonitor::StatisticsContainer::iterator  findCandidate();



protected :

    FWMEMORY_API MemoryMonitor(); // protected ctor (singleton)

    /**
     * @brief   a shared pointer to the class instance
     */
    static ::boost::shared_ptr< MemoryMonitor > m_instance;

    /*
     * @brief  helper to find the statictic iterator from a shared ptr
     * @warning weak_ptr are note comparable by default : hence m_satistics.find(weak_ptr) already fail !!!!!!
     */
    StatisticsContainer::iterator  findObject( ::fwTools::Object::sptr obj);

    /// store memory statistics
    StatisticsContainer m_statistics;

    /// Policy to apply on change
    ::boost::shared_ptr< ::fwMemory::IDumpPolicy >    m_policy ;

};

} // namespace fwMemory

#endif // _FWMEMORY_MEMORYMONITOR_HPP_
