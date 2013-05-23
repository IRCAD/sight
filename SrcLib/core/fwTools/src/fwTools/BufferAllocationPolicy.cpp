/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/Exception.hpp>

#include "fwTools/ByteSize.hpp"
#include "fwTools/BufferAllocationPolicy.hpp"

namespace fwTools
{


void BufferMallocPolicy::allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception )
{
    buffer = malloc( size );
    if (buffer == NULL && size > 0)
    {
        FW_RAISE_EXCEPTION_MSG( ::fwTools::Exception,
                                "Cannot allocate memory ("
                                << ::fwTools::ByteSize(::fwTools::ByteSize::SizeType(size)) <<").");
    }
}

//------------------------------------------------------------------------------

void BufferMallocPolicy::reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception )
{
    buffer = realloc( buffer, size );
    if (buffer == NULL && size > 0)
    {
        FW_RAISE_EXCEPTION_MSG( ::fwTools::Exception,
                                "Cannot allocate memory ("
                                << ::fwTools::ByteSize(::fwTools::ByteSize::SizeType(size)) <<").");
    }
}

//------------------------------------------------------------------------------

void BufferMallocPolicy::destroy(BufferType &buffer)
{
    free( buffer );
    buffer = 0;
}

//------------------------------------------------------------------------------


BufferAllocationPolicy::sptr BufferMallocPolicy::New()
{
    return BufferAllocationPolicy::sptr(new BufferMallocPolicy);
}

//------------------------------------------------------------------------------





void BufferNewPolicy::allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception )
{
    try
    {
        buffer = new char[size];
    }
    catch (std::bad_alloc& ba)
    {
        FW_RAISE_EXCEPTION_MSG( ::fwTools::Exception,
                                "bad_alloc caught: " << ba.what());
    }
}

//------------------------------------------------------------------------------

void BufferNewPolicy::reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception )
{
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
    FW_RAISE_EXCEPTION_MSG( ::fwTools::Exception,
                            "Reallocation not managed for buffer allocated with 'new' operator.");
}

//------------------------------------------------------------------------------

void BufferNewPolicy::destroy(BufferType &buffer)
{
    delete[] static_cast<char*>(buffer);
    buffer = 0;
}

//------------------------------------------------------------------------------


BufferAllocationPolicy::sptr BufferNewPolicy::New()
{
    return BufferAllocationPolicy::sptr(new BufferNewPolicy);
}

//------------------------------------------------------------------------------





void BufferNoAllocPolicy::allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception )
{
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
    FW_RAISE_EXCEPTION_MSG( ::fwTools::Exception,
                            "No Allocation Policy should not be called.");
}

//------------------------------------------------------------------------------

void BufferNoAllocPolicy::reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception )
{
    FwCoreNotUsedMacro(buffer);
    FwCoreNotUsedMacro(size);
    FW_RAISE_EXCEPTION_MSG( ::fwTools::Exception,
                            "No Allocation Policy should not be called.");
}

//------------------------------------------------------------------------------

void BufferNoAllocPolicy::destroy(BufferType &buffer)
{
    FwCoreNotUsedMacro(buffer);
    SLM_ASSERT("No Alloc Policy should not be called", 0);
}

//------------------------------------------------------------------------------


BufferAllocationPolicy::sptr BufferNoAllocPolicy::New()
{
    return BufferAllocationPolicy::sptr(new BufferNoAllocPolicy);
}

//------------------------------------------------------------------------------




} //namespace fwTools


