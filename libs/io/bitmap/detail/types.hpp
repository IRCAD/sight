/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#pragma once

#include "io/bitmap/Writer.hpp"

namespace sight::io::bitmap::detail
{

enum class ExtendedMode : std::uint8_t
{
    DEFAULT  = std::uint8_t(Writer::Mode::DEFAULT),
    FAST     = std::uint8_t(Writer::Mode::FAST),
    BEST     = std::uint8_t(Writer::Mode::BEST),
    J2K_FAST = BEST + 100,
    J2K_BEST = BEST + 101
};

} // namespace sight::io::bitmap::detail
