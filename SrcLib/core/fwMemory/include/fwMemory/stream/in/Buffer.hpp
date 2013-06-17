/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_STREAM_IN_BUFFER_HPP__
#define __FWMEMORY_STREAM_IN_BUFFER_HPP__

#include <fwCore/macros.hpp>

#include "fwMemory/stream/in/IFactory.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

class FWMEMORY_CLASS_API Buffer : public IFactory
{
public:
    Buffer(void* buf, size_t size);

protected:

    FWMEMORY_API SPTR(std::istream) get();

    void* m_buf;
    size_t m_size;
};



} // namespace in
} // namespace stream
} // namespace fwMemory

#endif // __FWMEMORY_STREAM_IN_BUFFER_HPP__


