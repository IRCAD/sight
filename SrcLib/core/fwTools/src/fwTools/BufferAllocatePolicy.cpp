/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/BufferAllocationPolicy.hpp"

namespace fwTools
{


void BufferMallocPolicy::allocate(void *&buffer, BufferAllocationPolicy::SizeType size)
{
    buffer = malloc( size );
}

//------------------------------------------------------------------------------

void BufferMallocPolicy::reallocate(void *&buffer, BufferAllocationPolicy::SizeType size)
{
    buffer = realloc( buffer, size );
}

//------------------------------------------------------------------------------

void BufferMallocPolicy::destroy(void *&buffer)
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



void BufferNewPolicy::allocate(void *&buffer, BufferAllocationPolicy::SizeType size)
{
    buffer = new char[size];
}

//------------------------------------------------------------------------------

void BufferNewPolicy::reallocate(void *&buffer, BufferAllocationPolicy::SizeType size)
{
    SLM_ASSERT("reallocation not managed for buffer allocated with 'new'", 0);
}

//------------------------------------------------------------------------------

void BufferNewPolicy::destroy(void *&buffer)
{
    delete[] static_cast<char*>(buffer);
}

//------------------------------------------------------------------------------


BufferAllocationPolicy::sptr BufferNewPolicy::New()
{
    return BufferAllocationPolicy::sptr(new BufferNewPolicy);
}

//------------------------------------------------------------------------------



} //namespace fwTools


