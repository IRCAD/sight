/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
