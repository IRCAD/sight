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
class FWMEMORY_CLASS_API ValveDump : public fwMemory::IPolicy
{
public:
    fwCoreClassMacro(ValveDump, fwMemory::IPolicy, ::fwMemory::policy::factory::New< ValveDump >);

    FWMEMORY_API ValveDump();

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

    FWMEMORY_API bool setParam(const std::string& name, const std::string& value) override;
    FWMEMORY_API virtual std::string getParam(const std::string& name, bool* ok = NULL ) const override;
    FWMEMORY_API const ::fwMemory::IPolicy::ParamNamesType& getParamNames() const override;

protected:

    FWMEMORY_API bool needDump(size_t supplement) const;

    FWMEMORY_API size_t dump(size_t nbOfBytes);

    FWMEMORY_API void apply(size_t supplement = 0);

    size_t m_minFreeMem;
    size_t m_hysteresisOffset;
};

} // namespace policy

} // namespace fwMemory
