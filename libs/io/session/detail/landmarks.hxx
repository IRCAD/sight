/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/landmarks.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::landmarks
{

constexpr static auto s_Size {"Size"};
constexpr static auto s_Shape {"Shape"};
constexpr static auto s_Visibility {"Visibility"};
constexpr static auto s_Red {"Red"};
constexpr static auto s_Green {"Green"};
constexpr static auto s_Blue {"Blue"};
constexpr static auto s_Alpha {"Alpha"};
constexpr static auto s_Color {"Color"};
constexpr static auto s_X {"X"};
constexpr static auto s_Y {"Y"};
constexpr static auto s_Z {"Z"};
constexpr static auto s_Point {"Point"};
constexpr static auto s_Points {"Points"};
constexpr static auto s_Groups {"Groups"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
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

    for(const auto& name : landmarks->getGroupNames())
    {
        // Serialize landmark group
        const auto& group = landmarks->getGroup(name);

        boost::property_tree::ptree group_tree;
        group_tree.put(s_Size, group.m_size);

        switch(group.m_shape)
        {
            case data::landmarks::Shape::CUBE:
                group_tree.put(s_Shape, 0);
                break;

            case data::landmarks::Shape::SPHERE:
                group_tree.put(s_Shape, 1);
                break;
        }

        group_tree.put(s_Visibility, group.m_visibility);

        // Group color
        boost::property_tree::ptree color_tree;
        color_tree.put(s_Red, group.m_color[0]);
        color_tree.put(s_Green, group.m_color[1]);
        color_tree.put(s_Blue, group.m_color[2]);
        color_tree.put(s_Alpha, group.m_color[3]);
        group_tree.add_child(s_Color, color_tree);

        // Points
        boost::property_tree::ptree points_tree;

        for(const auto& point : group.m_points)
        {
            boost::property_tree::ptree point_tree;
            point_tree.put(s_X, point[0]);
            point_tree.put(s_Y, point[1]);
            point_tree.put(s_Z, point[2]);
            points_tree.add_child(s_Point, point_tree);
        }

        group_tree.add_child(s_Points, points_tree);
        groups_tree.add_child(name, group_tree);
    }

    _tree.add_child(s_Groups, groups_tree);
}

//------------------------------------------------------------------------------

inline static data::landmarks::sptr read(
    zip::ArchiveReader& /*unused*/,
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
    for(const auto& group_tree : _tree.get_child(s_Groups))
    {
        const auto& color_tree           = group_tree.second.get_child(s_Color);
        const std::array<float, 4> color = {
            color_tree.get<float>(s_Red),
            color_tree.get<float>(s_Green),
            color_tree.get<float>(s_Blue),
            color_tree.get<float>(s_Alpha),
        };

        landmarks->addGroup(
            group_tree.first,
            color,
            group_tree.second.get<float>(s_Size),
            [&]
            {
                const int shape = group_tree.second.get<int>(s_Shape);
                switch(shape)
                {
                    case 0:
                        return data::landmarks::Shape::CUBE;

                    case 1:
                        return data::landmarks::Shape::SPHERE;

                    default:
                        SIGHT_THROW("Unknown landmark shape: " << shape);
                }
            }(),
            group_tree.second.get<bool>(s_Visibility)
        );

        const auto& points_tree = group_tree.second.get_child(s_Points);
        for(const auto& point_tree : points_tree)
        {
            const std::array<double, 3> point = {
                point_tree.second.get<double>(s_X),
                point_tree.second.get<double>(s_Y),
                point_tree.second.get<double>(s_Z)
            };

            landmarks->addPoint(group_tree.first, point);
        }
    }

    return landmarks;
}

SIGHT_REGISTER_SERIALIZER(data::landmarks, write, read);

} // namespace sight::io::session::detail::landmarks
