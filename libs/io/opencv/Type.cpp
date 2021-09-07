/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "Type.hpp"

#include <opencv2/core.hpp>

namespace sight::io::opencv
{

//------------------------------------------------------------------------------

std::int32_t Type::toCv(const core::tools::Type _type, const size_t _numComponents)
{
    SIGHT_ASSERT("Number of dimensions should be between 1 and 4", _numComponents > 0 && _numComponents <= 4);

    static const std::map<core::tools::Type, std::array<std::int32_t, 4> > s_IMAGE_FORMAT_TO_CV =
    {{
        {core::tools::Type::s_UINT8, {{CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4}}},
        {core::tools::Type::s_INT8, {{CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4}}},
        {core::tools::Type::s_UINT16, {{CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4}}},
        {core::tools::Type::s_INT16, {{CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4}}},
        {core::tools::Type::s_INT32, {{CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4}}},
        {core::tools::Type::s_FLOAT, {{CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4}}},
        {core::tools::Type::s_DOUBLE, {{CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4}}},
    }
    };

    const auto it = s_IMAGE_FORMAT_TO_CV.find(_type);
    SIGHT_ASSERT("Format not handled by OpenCV: " + _type.string(), it != s_IMAGE_FORMAT_TO_CV.end());

    return (it->second)[_numComponents - 1];
}

//------------------------------------------------------------------------------

std::pair<core::tools::Type, uint8_t> Type::fromCv(int32_t _cvType)
{
    static const std::map<std::int32_t, std::pair<core::tools::Type, size_t> > s_IMAGE_FORMAT_FROM_CV =
    {{
        {CV_8UC1, {core::tools::Type::s_UINT8, 1}},
        {CV_8UC2, {core::tools::Type::s_UINT8, 2}},
        {CV_8UC3, {core::tools::Type::s_UINT8, 3}},
        {CV_8UC4, {core::tools::Type::s_UINT8, 4}},
        {CV_8SC1, {core::tools::Type::s_INT8, 1}},
        {CV_8SC2, {core::tools::Type::s_INT8, 2}},
        {CV_8SC3, {core::tools::Type::s_INT8, 3}},
        {CV_8SC4, {core::tools::Type::s_INT8, 4}},
        {CV_16UC1, {core::tools::Type::s_UINT16, 1}},
        {CV_16UC2, {core::tools::Type::s_UINT16, 2}},
        {CV_16UC3, {core::tools::Type::s_UINT16, 3}},
        {CV_16UC4, {core::tools::Type::s_UINT16, 4}},
        {CV_16SC1, {core::tools::Type::s_INT16, 1}},
        {CV_16SC2, {core::tools::Type::s_INT16, 2}},
        {CV_16SC3, {core::tools::Type::s_INT16, 3}},
        {CV_16SC4, {core::tools::Type::s_INT16, 4}},
        {CV_32SC1, {core::tools::Type::s_INT32, 1}},
        {CV_32SC2, {core::tools::Type::s_INT32, 2}},
        {CV_32SC3, {core::tools::Type::s_INT32, 3}},
        {CV_32SC4, {core::tools::Type::s_INT32, 4}},
        {CV_32FC1, {core::tools::Type::s_FLOAT, 1}},
        {CV_32FC2, {core::tools::Type::s_FLOAT, 2}},
        {CV_32FC3, {core::tools::Type::s_FLOAT, 3}},
        {CV_32FC4, {core::tools::Type::s_FLOAT, 4}},
        {CV_64FC1, {core::tools::Type::s_DOUBLE, 1}},
        {CV_64FC2, {core::tools::Type::s_DOUBLE, 2}},
        {CV_64FC3, {core::tools::Type::s_DOUBLE, 3}},
        {CV_64FC4, {core::tools::Type::s_DOUBLE, 4}}
    }
    };

    const auto it = s_IMAGE_FORMAT_FROM_CV.find(_cvType);
    SIGHT_ASSERT("Format not handled by OpenCV: " + std::to_string(_cvType), it != s_IMAGE_FORMAT_FROM_CV.end());

    return it->second;
}

//------------------------------------------------------------------------------

} // namespace sight::io::opencv
