/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "core/memory/stream/in/RawZ.hpp"

#include <core/macros.hpp>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <filesystem>
#include <fstream>

namespace sight::core::memory
{

namespace stream
{

namespace in
{

struct FilteringStream : boost::iostreams::filtering_istream
{
    ~FilteringStream()
    {
        try
        {
            this->reset();
        }
        catch(...)
        {
        }
    }

    SPTR(void) heldStream;
};

SPTR(std::istream) RawZ::get()
{
    SPTR(std::ifstream) fs =
        std::make_shared<std::ifstream>(m_path, std::ios::in | std::ios::binary);

    SPTR(FilteringStream) filter = std::make_shared<FilteringStream>();

    filter->heldStream = fs;

    filter->push(boost::iostreams::gzip_decompressor());
    filter->push(*fs);

    return filter;
}

} // namespace in

} // namespace stream

} // namespace sight::core::memory
