/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "io/session/detail/Helper.hpp"

#include <data/Color.hpp>
#include <data/Image.hpp>
#include <data/Matrix4.hpp>

namespace sight::io::session
{

namespace detail::Matrix4
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& = ""
)
{
    const auto matrix = Helper::safeCast<data::Matrix4>(object);

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
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto matrix = Helper::safeCast<data::Matrix4>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Matrix4>(tree, 0, 1);

    std::array<double, 16> coefficients;
    for(std::size_t index = 0, end = coefficients.size() ; index < end ; ++index)
    {
        coefficients[index] = tree.get<double>(std::to_string(index));
    }

    matrix->setCoefficients(coefficients);

    return matrix;
}

} // namespace detail::Matrix4

} // namespace sight::io
