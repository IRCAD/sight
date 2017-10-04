/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_POLICY_BARRIERDUMP_HPP__
#define __FWMEMORY_POLICY_BARRIERDUMP_HPP__

#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/BufferManager.hpp"
#include "fwMemory/config.hpp"
#include "fwMemory/IPolicy.hpp"
#include "fwMemory/policy/factory/new.hpp"

#include <fwCore/base.hpp>

namespace fwMemory
{

namespace policy
{

/**
 * @brief Barrier dump policy
 *
 * This policy defines a memory usage barrier and will try to keep the managed
 * buffers memory usage under this barrier.
 */
class FWMEMORY_CLASS_API BarrierDump : public ::fwMemory::IPolicy
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((BarrierDump)(fwMemory::IPolicy),
                                           (()),
                                           ::fwMemory::policy::factory::New< BarrierDump >)
    FWMEMORY_API BarrierDump();

    FWMEMORY_API virtual void allocationRequest( BufferInfo& info,
                                                 ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                                 BufferInfo::SizeType size ) override;

    FWMEMORY_API virtual void setRequest( BufferInfo& info,
                                          ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                          BufferInfo::SizeType size ) override;

    FWMEMORY_API virtual void reallocateRequest( BufferInfo& info,
                                                 ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                                 BufferInfo::SizeType newSize ) override;

    FWMEMORY_API virtual void destroyRequest( BufferInfo& info,
                                              ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;

    FWMEMORY_API virtual void lockRequest( BufferInfo& info,
                                           ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;
    FWMEMORY_API virtual void unlockRequest( BufferInfo& info,
                                             ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;

    FWMEMORY_API virtual void dumpSuccess( BufferInfo& info,
                                           ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;
    FWMEMORY_API virtual void restoreSuccess( BufferInfo& info,
                                              ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;

    FWMEMORY_API virtual void refresh() override;

    //------------------------------------------------------------------------------

    void setBarrier( size_t barrier )
    {
        m_barrier = barrier;
    }
    //------------------------------------------------------------------------------

    size_t getBarrier() const
    {
        return m_barrier;
    }

    FWMEMORY_API virtual std::string getParam(const std::string& name, bool* ok = NULL ) const override;
    FWMEMORY_API bool setParam(const std::string& name, const std::string& value) override;
    FWMEMORY_API const fwMemory::IPolicy::ParamNamesType& getParamNames() const override;

protected:

    FWMEMORY_API size_t getTotalAlive() const;
    FWMEMORY_API bool isBarrierCrossed() const;

    FWMEMORY_API size_t dump(size_t nbOfBytes);

    FWMEMORY_API void apply();

    size_t m_totalAllocated;
    size_t m_totalDumped;
    size_t m_barrier;
};

} // namespace policy

} // namespace fwMemory

#endif // __FWMEMORY_POLICY_BARRIERDUMP_HPP__

