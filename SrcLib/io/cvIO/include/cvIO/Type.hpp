/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __CVIO_TYPE_HPP__
#define __CVIO_TYPE_HPP__

#include "cvIO/config.hpp"

#include <fwTools/Type.hpp>

namespace cvIO
{

class CVIO_CLASS_API Type
{
public:
    /// Returns an OpenCV format given a Sight pixel type and a number of components
    CVIO_API static std::int32_t toCv(const ::fwTools::Type _type, const size_t _numComponents);

    /// Returns a Sight pixel type and a number of components given an OpenCV format
    CVIO_API static std::pair< ::fwTools::Type, std::uint8_t> fromCv(std::int32_t _cvType);
};

} // namespace cvIO

#endif //__CVIO_TYPE_HPP__
