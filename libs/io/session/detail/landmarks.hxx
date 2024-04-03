/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/io/session/config.hpp>

#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/landmarks.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::landmarks
{

constexpr static auto SIZE {"Size"};
constexpr static auto SHAPE {"Shape"};
constexpr static auto VISIBILITY {"Visibility"};
constexpr static auto RED {"Red"};
constexpr static auto GREEN {"Green"};
constexpr static auto BLUE {"Blue"};
constexpr static auto ALPHA {"Alpha"};
constexpr static auto COLOR {"Color"};
constexpr static auto X {"X"};
constexpr static auto Y {"Y"};
constexpr static auto Z {"Z"};
constexpr static auto POINT {"Point"};
constexpr static auto POINTS {"Points"};
constexpr static auto GROUPS {"Groups"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto landmarks = helper::safe_cast<data::landmarks>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::landmarks>(_tree, 1);

    boost::property_tree::ptree groups_tree;

    for(const auto& name : landmarks->get_group_names())
    {
        // Serialize landmark group
        const auto& group = landmarks->get_group(name);

        boost::property_tree::ptree group_tree;
        group_tree.put(SIZE, group.m_size);

        switch(group.m_shape)
        {
            case data::landmarks::shape::cube:
                group_tree.put(SHAPE, 0);
                break;

            case data::landmarks::shape::sphere:
                group_tree.put(SHAPE, 1);
                break;
        }

        group_tree.put(VISIBILITY, group.m_visibility);

        // Group color
        boost::property_tree::ptree color_tree;
        color_tree.put(RED, group.m_color[0]);
        color_tree.put(GREEN, group.m_color[1]);
        color_tree.put(BLUE, group.m_color[2]);
        color_tree.put(ALPHA, group.m_color[3]);
        group_tree.add_child(COLOR, color_tree);

        // Points
        boost::property_tree::ptree points_tree;

        for(const auto& point : group.m_points)
        {
            boost::property_tree::ptree point_tree;
            point_tree.put(X, point[0]);
            point_tree.put(Y, point[1]);
            point_tree.put(Z, point[2]);
            points_tree.add_child(POINT, point_tree);
        }

        group_tree.add_child(POINTS, points_tree);
        groups_tree.add_child(name, group_tree);
    }

    _tree.add_child(GROUPS, groups_tree);
}

//------------------------------------------------------------------------------

inline static data::landmarks::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto landmarks = helper::cast_or_create<data::landmarks>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::landmarks>(_tree, 0, 1);

    // Iterate on groups
    for(const auto& group_tree : _tree.get_child(GROUPS))
    {
        const auto& color_tree           = group_tree.second.get_child(COLOR);
        const std::array<float, 4> color = {
            color_tree.get<float>(RED),
            color_tree.get<float>(GREEN),
            color_tree.get<float>(BLUE),
            color_tree.get<float>(ALPHA),
        };

        landmarks->add_group(
            group_tree.first,
            color,
            group_tree.second.get<float>(SIZE),
            [&]
            {
                const int shape = group_tree.second.get<int>(SHAPE);
                switch(shape)
                {
                    case 0:
                        return data::landmarks::shape::cube;

                    case 1:
                        return data::landmarks::shape::sphere;

                    default:
                        SIGHT_THROW("Unknown landmark shape: " << shape);
                }
            }(),
            group_tree.second.get<bool>(VISIBILITY)
        );

        const auto& points_tree = group_tree.second.get_child(POINTS);
        for(const auto& point_tree : points_tree)
        {
            const std::array<double, 3> point = {
                point_tree.second.get<double>(X),
                point_tree.second.get<double>(Y),
                point_tree.second.get<double>(Z)
            };

            landmarks->add_point(group_tree.first, point);
        }
    }

    return landmarks;
}

SIGHT_REGISTER_SERIALIZER(data::landmarks, write, read);

} // namespace sight::io::session::detail::landmarks
