/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwMemory/policy/NeverDump.hpp>


namespace fwMemory
{

namespace policy
{


NeverDump::sptr NeverDump::New()
{
    return NeverDump::sptr(new NeverDump());
}

//------------------------------------------------------------------------------

void NeverDump::allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
}

//------------------------------------------------------------------------------


void NeverDump::setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
}

//------------------------------------------------------------------------------


void NeverDump::reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize )
{
}

//------------------------------------------------------------------------------


void NeverDump::destroyRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::lockRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::unlockRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::dumpSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void NeverDump::restoreSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------



void NeverDump::setManager(::fwTools::IBufferManager::sptr manager)
{
}

//------------------------------------------------------------------------------


} // namespace policy

} //namespace fwMemory
