/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_CAMP_DECLARE_HPP__
#define __FWMEMORY_CAMP_DECLARE_HPP__

#include <fwCamp/macros.hpp>

#include "fwMemory/BufferObject.hpp"

namespace fwMemory
{
namespace camp
{

struct runner
{
    runner()
    {
        localDeclarefwMemoryBufferObject();
    }

    static runner r;
};

} // namespace camp
} // namespace fwMemory

#endif //__FWMEMORY_CAMP_DECLARE_HPP__
