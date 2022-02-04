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
#include "core/memory/tools/MemoryMonitorTools.hpp"

#include <core/base.hpp>

namespace sight::core::memory
{

namespace policy
{

/**
 * @brief Valve dump policy
 *
 * This policy is monitoring system memory usage and will trigger memory dump
 * when the amount of system free memory is lower than the minFreeMem parameter.
 *
 * An hysteresis parameter exists to free more memory when the process is
 * triggered. If the free system memory amount is lower than the minFreeMem,
 * this policy will try to reach minFreeMem + hysteresisOffset bytes of free
 * memory.
 */
template<typename TOOL = tools::MemoryMonitorTools>
class ValveDump : public core::memory::IPolicy
{
public:

    SIGHT_DECLARE_CLASS(ValveDump, core::memory::IPolicy, core::memory::policy::factory::New<ValveDump>);

    ValveDump();

    virtual void allocationRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType size
    ) override;
    virtual void setRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType size
    ) override;
    virtual void reallocateRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType newSize
    ) override;

    virtual void destroyRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    virtual void lockRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;
    virtual void unlockRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    virtual void dumpSuccess(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;
    virtual void restoreSuccess(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    void refresh() override;

    bool setParam(const std::string& name, const std::string& value) override;
    std::string getParam(const std::string& name, bool* ok = NULL) const override;
    const core::memory::IPolicy::ParamNamesType& getParamNames() const override;

protected:

    bool needDump(std::size_t supplement) const;

    std::size_t dump(std::size_t nbOfBytes);

    void apply(std::size_t supplement = 0);

    std::size_t m_minFreeMem;
    std::size_t m_hysteresisOffset;
};

} // namespace policy

} // namespace sight::core::memory

#include "ValveDump.hxx"
