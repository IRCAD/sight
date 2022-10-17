/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/memory/BufferInfo.hpp"
#include "core/memory/BufferManager.hpp"
#include "core/memory/IPolicy.hpp"
#include "core/memory/policy/factory/new.hpp"

#include <core/base.hpp>

namespace sight::core::memory::policy
{

/**
 * @brief Barrier dump policy
 *
 * This policy defines a memory usage barrier and will try to keep the managed
 * buffers memory usage under this barrier.
 */
class CORE_CLASS_API BarrierDump : public core::memory::IPolicy
{
public:

    SIGHT_DECLARE_CLASS(BarrierDump, core::memory::IPolicy, core::memory::policy::factory::New<BarrierDump>);

    CORE_API BarrierDump();

    CORE_API void allocationRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType size
    ) override;

    CORE_API void setRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType size
    ) override;

    CORE_API void reallocateRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType newSize
    ) override;

    CORE_API void destroyRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    CORE_API void lockRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;
    CORE_API void unlockRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    CORE_API void dumpSuccess(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;
    CORE_API void restoreSuccess(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    CORE_API void refresh() override;

    //------------------------------------------------------------------------------

    void setBarrier(std::size_t barrier)
    {
        m_barrier = barrier;
    }

    //------------------------------------------------------------------------------

    std::size_t getBarrier() const
    {
        return m_barrier;
    }

    CORE_API std::string getParam(const std::string& name, bool* ok = nullptr) const override;
    CORE_API bool setParam(const std::string& name, const std::string& value) override;
    CORE_API const core::memory::IPolicy::ParamNamesType& getParamNames() const override;

protected:

    CORE_API std::size_t getTotalAlive() const;
    CORE_API bool isBarrierCrossed() const;

    static CORE_API std::size_t dump(std::size_t nbOfBytes);

    CORE_API void apply();

    std::size_t m_totalAllocated {0};
    std::size_t m_totalDumped {0};
    std::size_t m_barrier;
};

} // namespace sight::core::memory::policy
