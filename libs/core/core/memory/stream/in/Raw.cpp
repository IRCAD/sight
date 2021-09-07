/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/memory/stream/in/Raw.hpp"

#include <core/exceptionmacros.hpp>
#include <core/macros.hpp>

#include <filesystem>
#include <fstream>

namespace sight::core::memory
{

namespace stream
{

namespace in
{

SPTR(std::istream) Raw::get()
{
    SIGHT_THROW_IF(
        "file " << m_path.string() << " does not exist anymore or has been moved.",
        !std::filesystem::exists(m_path)
    );

    SPTR(std::ifstream) fs =
        std::make_shared<std::ifstream>(m_path.string(), std::ios::in | std::ios::binary);
    return fs;
}

} // namespace in

} // namespace stream

} // namespace sight::core::memory
