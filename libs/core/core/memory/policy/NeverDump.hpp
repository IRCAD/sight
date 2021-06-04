/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "core/memory/IPolicy.hpp"
#include "core/memory/policy/factory/new.hpp"
#include <core/base.hpp>

namespace sight::core::memory
{

namespace policy
{

/**
 * @brief Never dump policy
 *
 * This policy will never take the initiative to free memory. This is the policy
 * used when no automatic memory management is wanted. Memory will be dumped on
 * demand.
 */
class CORE_CLASS_API NeverDump : public core::memory::IPolicy
{
public:

    SIGHT_DECLARE_CLASS(NeverDump, core::memory::IPolicy, core::memory::policy::factory::New<NeverDump>);

    CORE_API virtual void allocationRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType size
    ) override;

    CORE_API virtual void setRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType size
    ) override;

    CORE_API virtual void reallocateRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer,
        BufferInfo::SizeType newSize
    ) override;

    CORE_API virtual void destroyRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    CORE_API virtual void lockRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;
    CORE_API virtual void unlockRequest(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    CORE_API virtual void dumpSuccess(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;
    CORE_API virtual void restoreSuccess(
        BufferInfo& info,
        core::memory::BufferManager::ConstBufferPtrType buffer
    ) override;

    CORE_API void refresh() override;

    //------------------------------------------------------------------------------

    bool setParam(const std::string& name, const std::string& value) override
    {
        SIGHT_NOT_USED(name);
        SIGHT_NOT_USED(value);
        return false;
    }

    CORE_API std::string getParam(const std::string& name, bool* ok = NULL) const override;

    //------------------------------------------------------------------------------

    const core::memory::IPolicy::ParamNamesType& getParamNames() const override
    {
        static core::memory::IPolicy::ParamNamesType names;
        return names;
    }
};

} // namespace policy

} // namespace sight::core::memory
