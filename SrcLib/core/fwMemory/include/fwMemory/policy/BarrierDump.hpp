/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

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
