/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>


#include <fwCore/macros.hpp>
#include <fwCore/exceptionmacros.hpp>

#include "fwMemory/stream/in/Raw.hpp"

namespace fwMemory
{
namespace stream
{
namespace in
{

SPTR(std::istream) Raw::get()
{
    FW_RAISE_IF("file "<< m_path.string() << " does not exist anymore or has been moved.",
                !boost::filesystem::exists(m_path));

    SPTR(::boost::filesystem::ifstream) fs
        = std::make_shared< ::boost::filesystem::ifstream>(m_path, std::ios::in|std::ios::binary);
    return fs;
}

} // namespace in
} // namespace stream
} // namespace fwMemory

