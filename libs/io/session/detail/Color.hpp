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

namespace sight::io::session
{

namespace detail::Color
{

constexpr static auto s_Red {"Red"};
constexpr static auto s_Green {"Green"};
constexpr static auto s_Blue {"Blue"};
constexpr static auto s_Alpha {"Alpha"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto color = Helper::safeCast<data::Color>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Color>(tree, 1);

    tree.put(s_Red, color->red());
    tree.put(s_Green, color->green());
    tree.put(s_Blue, color->blue());
    tree.put(s_Alpha, color->alpha());
}

//------------------------------------------------------------------------------

inline static data::Color::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto color = Helper::safeCast<data::Color>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Color>(tree, 0, 1);

    color->setRGBA(
        tree.get<float>(s_Red),
        tree.get<float>(s_Green),
        tree.get<float>(s_Blue),
        tree.get<float>(s_Alpha)
    );

    return color;
}

} // namespace detail::Color

} // namespace sight::io
