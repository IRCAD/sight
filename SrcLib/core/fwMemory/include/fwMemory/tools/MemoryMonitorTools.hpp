/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_MEMORYMONITORTOOLS_HPP_
#define _FWMEMORY_MEMORYMONITORTOOLS_HPP_

#include <boost/cstdint.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{
namespace tools
{

class FWMEMORY_CLASS_API MemoryMonitorTools
{

public:

    FWMEMORY_API MemoryMonitorTools() ;

    FWMEMORY_API ~MemoryMonitorTools() ;


    FWMEMORY_API static ::boost::uint64_t estimateFreeMem();


    FWMEMORY_API static void printProcessMemoryInformation();

    FWMEMORY_API static void printSystemMemoryInformation();

    FWMEMORY_API static void printMemoryInformation();


    FWMEMORY_API static ::boost::uint64_t getTotalSystemMemory();

    FWMEMORY_API static ::boost::uint64_t getUsedSystemMemory();

    FWMEMORY_API static ::boost::uint64_t getFreeSystemMemory();


    FWMEMORY_API static ::boost::uint64_t getUsedProcessMemory();


};

} // namespace tools
} // namespace fwMemory

#endif // _FWMEMORY_MEMORYMONITORTOOLS_HPP_
