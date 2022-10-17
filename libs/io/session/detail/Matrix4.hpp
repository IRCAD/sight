/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "io/session/config.hpp"
#include "io/session/Helper.hpp"

#include <data/Color.hpp>
#include <data/Image.hpp>
#include <data/Matrix4.hpp>

namespace sight::io::session::detail::Matrix4
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto matrix = Helper::safe_cast<data::Matrix4>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Matrix4>(tree, 1);

    std::size_t index = 0;
    for(const auto& coefficient : matrix->getCoefficients())
    {
        tree.put(std::to_string(index++), coefficient);
    }
}

//------------------------------------------------------------------------------

inline static data::Matrix4::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto matrix = Helper::cast_or_create<data::Matrix4>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Matrix4>(tree, 0, 1);

    std::array<double, 16> coefficients {};
    for(std::size_t index = 0, end = coefficients.size() ; index < end ; ++index)
    {
        coefficients[index] = tree.get<double>(std::to_string(index));
    }

    matrix->setCoefficients(coefficients);

    return matrix;
}

} // namespace sight::io::session::detail::Matrix4
