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
#include "io/session/macros.hpp"

#include <data/Landmarks.hpp>
#include <data/Point.hpp>

namespace sight::io::session::detail::Landmarks
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
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto landmarks = Helper::safe_cast<data::Landmarks>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Landmarks>(tree, 1);

    boost::property_tree::ptree groupsTree;

    for(const auto& name : landmarks->getGroupNames())
    {
        // Serialize landmark group
        const auto& group = landmarks->getGroup(name);

        boost::property_tree::ptree groupTree;
        groupTree.put(s_Size, group.m_size);

        switch(group.m_shape)
        {
            case data::Landmarks::Shape::CUBE:
                groupTree.put(s_Shape, 0);
                break;

            case data::Landmarks::Shape::SPHERE:
                groupTree.put(s_Shape, 1);
                break;
        }

        groupTree.put(s_Visibility, group.m_visibility);

        // Group color
        boost::property_tree::ptree colorTree;
        colorTree.put(s_Red, group.m_color[0]);
        colorTree.put(s_Green, group.m_color[1]);
        colorTree.put(s_Blue, group.m_color[2]);
        colorTree.put(s_Alpha, group.m_color[3]);
        groupTree.add_child(s_Color, colorTree);

        // Points
        boost::property_tree::ptree pointsTree;

        for(const auto& point : group.m_points)
        {
            boost::property_tree::ptree pointTree;
            pointTree.put(s_X, point[0]);
            pointTree.put(s_Y, point[1]);
            pointTree.put(s_Z, point[2]);
            pointsTree.add_child(s_Point, pointTree);
        }

        groupTree.add_child(s_Points, pointsTree);
        groupsTree.add_child(name, groupTree);
    }

    tree.add_child(s_Groups, groupsTree);
}

//------------------------------------------------------------------------------

inline static data::Landmarks::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto landmarks = Helper::cast_or_create<data::Landmarks>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Landmarks>(tree, 0, 1);

    // Iterate on groups
    for(const auto& groupTree : tree.get_child(s_Groups))
    {
        const auto& colorTree            = groupTree.second.get_child(s_Color);
        const std::array<float, 4> color = {
            colorTree.get<float>(s_Red),
            colorTree.get<float>(s_Green),
            colorTree.get<float>(s_Blue),
            colorTree.get<float>(s_Alpha),
        };

        landmarks->addGroup(
            groupTree.first,
            color,
            groupTree.second.get<float>(s_Size),
            [&]
            {
                const int shape = groupTree.second.get<int>(s_Shape);
                switch(shape)
                {
                    case 0:
                        return data::Landmarks::Shape::CUBE;

                    case 1:
                        return data::Landmarks::Shape::SPHERE;

                    default:
                        SIGHT_THROW("Unknown landmark shape: " << shape);
                }
            }(),
            groupTree.second.get<bool>(s_Visibility)
        );

        const auto& pointsTree = groupTree.second.get_child(s_Points);
        for(const auto& pointTree : pointsTree)
        {
            const std::array<double, 3> point = {
                pointTree.second.get<double>(s_X),
                pointTree.second.get<double>(s_Y),
                pointTree.second.get<double>(s_Z)
            };

            landmarks->addPoint(groupTree.first, point);
        }
    }

    return landmarks;
}

SIGHT_REGISTER_SERIALIZER(data::Landmarks, write, read);

} // namespace sight::io::session::detail::Landmarks
