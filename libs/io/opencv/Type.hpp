/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "io/opencv/config.hpp"

#include <core/Type.hpp>

namespace sight::io::opencv
{

class IO_OPENCV_CLASS_API Type
{
public:

    /// Returns an OpenCV format given a Sight pixel type and a number of components
    IO_OPENCV_API static std::int32_t toCv(const core::Type _type, const std::size_t _numComponents);

    /// Returns a Sight pixel type and a number of components given an OpenCV format
    IO_OPENCV_API static std::pair<core::Type, std::uint8_t> fromCv(std::int32_t _cvType);
};

} // namespace sight::io::opencv
