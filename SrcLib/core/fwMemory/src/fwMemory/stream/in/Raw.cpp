/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwMemory/stream/in/Raw.hpp"

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/macros.hpp>

#include <filesystem>

#include <fstream>

namespace fwMemory
{
namespace stream
{
namespace in
{

SPTR(std::istream) Raw::get()
{
    FW_RAISE_IF("file "<< m_path.string() << " does not exist anymore or has been moved.",
                !std::filesystem::exists(m_path));

    SPTR(std::ifstream) fs
        = std::make_shared< std::ifstream>(m_path, std::ios::in|std::ios::binary);
    return fs;
}

} // namespace in
} // namespace stream
} // namespace fwMemory
