/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMemory/stream/in/RawZ.hpp"

#include <fwCore/macros.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace fwMemory
{
namespace stream
{
namespace in
{

struct FilteringStream : ::boost::iostreams::filtering_istream
{

    ~FilteringStream()
    {
        try
        {
            this->reset();
        }
        catch (...)
        {
        }
    }

    SPTR(void) heldStream;
};

SPTR(std::istream) RawZ::get()
{
    SPTR(::boost::filesystem::ifstream) fs
        = std::make_shared< ::boost::filesystem::ifstream>(m_path, std::ios::in|std::ios::binary);

    SPTR(FilteringStream) filter = std::make_shared< FilteringStream >();

    filter->heldStream = fs;

    filter->push(::boost::iostreams::gzip_decompressor());
    filter->push(*fs);

    return filter;
}

} // namespace in
} // namespace stream
} // namespace fwMemory

