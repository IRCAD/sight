/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_EXCEPTION_MEMORY_HPP__
#define __FWMEMORY_EXCEPTION_MEMORY_HPP__

#include <fwCore/Exception.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{
namespace exception
{

/**
 * @brief   Implements an exception class for fwMemory
 * @class   Memory
 */
struct FWMEMORY_CLASS_API Memory : ::fwCore::Exception
{
    Memory( const std::string &err ) : ::fwCore::Exception(err)
    {
    }
};

} // namespace exception
} // namespace fwMemory

#endif // __FWMEMORY_EXCEPTION_MEMORY_HPP__

