/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/memory/stream/in/raw_z.hpp"

#include <core/macros.hpp>

#ifdef _MSC_VER
// warning for unreachable code in Release/RelWithDebInfo in boost::iostreams whereas it should be ignored,
// see https://developercommunity.visualstudio.com/t/error:-C4702-with-external:w0/1696694
#pragma warning(disable : 4702)
#endif // _MSC_VER

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <filesystem>
#include <fstream>

namespace sight::core::memory::stream::in
{

struct filtering_stream : boost::iostreams::filtering_istream
{
    ~filtering_stream() override
    {
        try
        {
            this->reset();
        }
        catch(...)
        {
        }
    }

    SPTR(void) held_stream;
};

SPTR(std::istream) raw_z::get()
{
    SPTR(std::ifstream) fs =
        std::make_shared<std::ifstream>(m_path, std::ios::in | std::ios::binary);

    SPTR(filtering_stream) filter = std::make_shared<filtering_stream>();

    filter->held_stream = fs;

    filter->push(boost::iostreams::gzip_decompressor());
    filter->push(*fs);

    return filter;
}

} // namespace sight::core::memory::stream::in
