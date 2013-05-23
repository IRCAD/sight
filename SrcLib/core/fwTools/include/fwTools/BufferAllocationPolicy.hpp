/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_BUFFERALLOCATIONPOLICY_HPP_
#define _FWTOOLS_BUFFERALLOCATIONPOLICY_HPP_

#include <fwCore/base.hpp>

#include "fwTools/Exception.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{

class FWTOOLS_CLASS_API BufferAllocationPolicy
{
public:
    typedef SPTR(BufferAllocationPolicy) sptr;
    typedef size_t SizeType;
    typedef void* BufferType;

    FWTOOLS_API virtual void allocate(BufferType &buffer, SizeType size) throw( ::fwTools::Exception ) = 0;
    FWTOOLS_API virtual void reallocate(BufferType &buffer, SizeType size) throw( ::fwTools::Exception ) = 0;
    FWTOOLS_API virtual void destroy(BufferType &buffer) = 0;

    FWTOOLS_API virtual ~BufferAllocationPolicy(){};
};


class FWTOOLS_CLASS_API BufferMallocPolicy : public BufferAllocationPolicy
{
public:
    FWTOOLS_API void allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception );
    FWTOOLS_API void reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception );
    FWTOOLS_API void destroy(BufferType &buffer);

    FWTOOLS_API static BufferAllocationPolicy::sptr New();
};


class FWTOOLS_CLASS_API BufferNewPolicy : public BufferAllocationPolicy
{
public:
    FWTOOLS_API void allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception );
    FWTOOLS_API void reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception );
    FWTOOLS_API void destroy(BufferType &buffer);

    FWTOOLS_API static BufferAllocationPolicy::sptr New();
};

class FWTOOLS_CLASS_API BufferNoAllocPolicy : public BufferAllocationPolicy
{
public:
    FWTOOLS_API void allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception );
    FWTOOLS_API void reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwTools::Exception );
    FWTOOLS_API void destroy(BufferType &buffer);

    FWTOOLS_API static BufferAllocationPolicy::sptr New();
};


}

#endif /* _FWTOOLS_BUFFERALLOCATIONPOLICY_HPP_ */
