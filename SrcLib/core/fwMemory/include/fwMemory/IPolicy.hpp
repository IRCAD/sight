/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWMEMORY_IPOLICY_HPP__
#define __FWMEMORY_IPOLICY_HPP__

#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/BufferManager.hpp"
#include "fwMemory/config.hpp"
#include "fwMemory/policy/factory/new.hpp"

#include <fwCore/base.hpp>

namespace fwMemory
{

/**
 * @brief Defines the memory dump policy interface
 */
class FWMEMORY_CLASS_API IPolicy : public fwCore::BaseObject
{
public:

    typedef SPTR (IPolicy) sptr;
    typedef std::vector<std::string> ParamNamesType;

    virtual void allocationRequest(BufferInfo& info,
                                   ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType size ) = 0;

    virtual void setRequest(BufferInfo& info,
                            ::fwMemory::BufferManager::ConstBufferPtrType buffer,  BufferInfo::SizeType size ) = 0;

    virtual void reallocateRequest(BufferInfo& info,
                                   ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType newSize ) = 0;

    virtual void destroyRequest(BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer ) = 0;

    virtual void lockRequest(BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )   = 0;
    virtual void unlockRequest(BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer ) = 0;

    virtual void dumpSuccess( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )    = 0;
    virtual void restoreSuccess( BufferInfo& info, ::fwMemory::BufferManager::ConstBufferPtrType buffer ) = 0;

    virtual void refresh() = 0;

    virtual bool setParam(const std::string& name, const std::string& value)      = 0;
    virtual std::string getParam(const std::string& name, bool* ok = NULL ) const = 0;
    virtual const ParamNamesType& getParamNames() const                           = 0;

    template <typename T>
    class Registrar
    {
    public:
        Registrar()
        {
            ::fwMemory::policy::registry::get()->addFactory(T::leafClassname(), &::fwMemory::policy::factory::New<T>);
        }
    };

};

} // namespace fwMemory

#endif // __FWMEMORY_IPOLICY_HPP__

