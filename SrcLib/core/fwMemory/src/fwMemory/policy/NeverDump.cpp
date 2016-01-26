/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwMemory/policy/registry/macros.hpp"
#include "fwMemory/policy/NeverDump.hpp"


namespace fwMemory
{

namespace policy
{

fwMemoryPolicyRegisterMacro(::fwMemory::policy::NeverDump);

//------------------------------------------------------------------------------

void NeverDump::allocationRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
}

//------------------------------------------------------------------------------


void NeverDump::setRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                            BufferInfo::SizeType size )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
}

//------------------------------------------------------------------------------


void NeverDump::reallocateRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                   BufferInfo::SizeType newSize )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(newSize);
}

//------------------------------------------------------------------------------


void NeverDump::destroyRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------


void NeverDump::lockRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------


void NeverDump::unlockRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::dumpSuccess( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::restoreSuccess( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    FwCoreNotUsedMacro(info);
    FwCoreNotUsedMacro(buffer);
}

//------------------------------------------------------------------------------

void NeverDump::refresh()
{
}

//------------------------------------------------------------------------------

std::string NeverDump::getParam(const std::string &name, bool *ok) const
{
    FwCoreNotUsedMacro(name);
    if (ok)
    {
        *ok = false;
    }
    return "";
}

//------------------------------------------------------------------------------



} // namespace policy

} //namespace fwMemory
