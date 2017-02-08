/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

