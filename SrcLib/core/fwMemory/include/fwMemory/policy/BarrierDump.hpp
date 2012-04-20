/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_BARRIERDUMP_HPP_
#define FWMEMORY_BARRIERDUMP_HPP_

#include <boost/shared_ptr.hpp>

#include <fwCore/base.hpp>

#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/BufferManager.hpp"
#include "fwMemory/IPolicy.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

namespace policy
{

/**
 * @brief Define the memory dump policy interface
 */
class FWMEMORY_CLASS_API BarrierDump : public fwMemory::IPolicy
{
public :

    typedef SPTR(BarrierDump) sptr;

    static sptr New();

    BarrierDump();

    virtual void allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size ) ;
    virtual void setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size ) ;
    virtual void reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize ) ;
    virtual void destroyRequest( BufferInfo &info, void **buffer ) ;
    virtual void lockRequest( BufferInfo &info, void **buffer ) ;
    virtual void unlockRequest( BufferInfo &info, void **buffer ) ;

    virtual void dumpSuccess( BufferInfo &info, void **buffer );
    virtual void restoreSuccess( BufferInfo &info, void **buffer );

    void setManager(::fwTools::IBufferManager::sptr manager);

protected :

    size_t getTotalAlive();
    bool isBarrierCrossed();

    size_t dump(size_t nbOfBytes);

    void apply();

    size_t m_totalAllocated;
    size_t m_totalDumped;
    size_t m_barrier;

    ::fwMemory::BufferManager::wptr m_manager;


};


} // namespace policy

} // namespace fwMemory

#endif // FWMEMORY_BARRIERDUMP_HPP_

