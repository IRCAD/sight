/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <fwCore/macros.hpp>
#include <fwCore/spyLog.hpp>

#include "fwMemory/stream/in/Buffer.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

Buffer::Buffer(void* buf, size_t size) :
    m_buf(buf), m_size(size)
{
    OSLM_ASSERT("Buffer is null.", m_buf);
};

SPTR(std::istream) Buffer::get()
{
    namespace io = boost::iostreams;

    typedef ::boost::iostreams::stream< ::boost::iostreams::array_source > ArrayStreamType;
    SPTR( ArrayStreamType ) arrayInStream = ::boost::make_shared< ArrayStreamType > ( static_cast<char *>(m_buf), m_size );

    return arrayInStream;
}

} // namespace in
} // namespace stream
} // namespace fwMemory

