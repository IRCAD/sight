/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_BUFFERALLOCATIONPOLICY_HPP_
#define _FWTOOLS_BUFFERALLOCATIONPOLICY_HPP_

#include <fwCore/base.hpp>

#include "fwTools/config.hpp"

namespace fwTools
{

class FWTOOLS_CLASS_API BufferAllocationPolicy
{
public:
    typedef SPTR(BufferAllocationPolicy) sptr;
    typedef size_t SizeType;

    FWTOOLS_API virtual void allocate(void *&buffer, SizeType size) = 0;
    FWTOOLS_API virtual void reallocate(void *&buffer, SizeType size) = 0;
    FWTOOLS_API virtual void destroy(void *&buffer) = 0;
};


class FWTOOLS_CLASS_API BufferMallocPolicy : public BufferAllocationPolicy
{
public:
    FWTOOLS_API void allocate(void *&buffer, BufferAllocationPolicy::SizeType size);
    FWTOOLS_API void reallocate(void *&buffer, BufferAllocationPolicy::SizeType size);
    FWTOOLS_API void destroy(void *&buffer);

    FWTOOLS_API static BufferAllocationPolicy::sptr New();
};


class FWTOOLS_CLASS_API BufferNewPolicy : public BufferAllocationPolicy
{
public:
    FWTOOLS_API void allocate(void *&buffer, BufferAllocationPolicy::SizeType size);
    FWTOOLS_API void reallocate(void *&buffer, BufferAllocationPolicy::SizeType size);
    FWTOOLS_API void destroy(void *&buffer);

    FWTOOLS_API static BufferAllocationPolicy::sptr New();
};

}

#endif /* _FWTOOLS_BUFFERALLOCATIONPOLICY_HPP_ */
