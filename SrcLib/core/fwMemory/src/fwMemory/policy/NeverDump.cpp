/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwMemory/policy/NeverDump.hpp"


namespace fwMemory
{

namespace policy
{

static IPolicy::Register<NeverDump> registerFactory(NeverDump::leafClassname());

//------------------------------------------------------------------------------

void NeverDump::allocationRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer, BufferInfo::SizeType size )
{
}

//------------------------------------------------------------------------------


void NeverDump::setRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer, BufferInfo::SizeType size )
{
}

//------------------------------------------------------------------------------


void NeverDump::reallocateRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer, BufferInfo::SizeType newSize )
{
}

//------------------------------------------------------------------------------


void NeverDump::destroyRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::lockRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::unlockRequest( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::dumpSuccess( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::restoreSuccess( BufferInfo &info, ::fwMemory::BufferManager::BufferPtrType buffer )
{
}

//------------------------------------------------------------------------------



void NeverDump::setManager(const ::fwMemory::BufferManager::sptr &manager)
{
}

//------------------------------------------------------------------------------

void NeverDump::refresh()
{}

//------------------------------------------------------------------------------

std::string NeverDump::getParam(const std::string &name, bool *ok  )
{
    if (ok)
    {
        *ok = false;
    }
    return "";
}

//------------------------------------------------------------------------------



} // namespace policy

} //namespace fwMemory
