/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_STREAM_IN_IFACTORY_HPP__
#define __FWMEMORY_STREAM_IN_IFACTORY_HPP__

#include <istream>
#include <fwCore/macros.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

class FWMEMORY_CLASS_API IFactory
{
public:
    virtual ~IFactory()
    {
    }

    SPTR(std::istream) operator()(){
        return this->get();
    }

protected:

    virtual SPTR(std::istream) get() = 0;

};

} // namespace in
} // namespace stream
} // namespace fwMemory

#endif // __FWMEMORY_STREAM_IN_IFACTORY_HPP__


