/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "data/Landmarks.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::Landmarks);

namespace sight::data
{

const core::com::signals::key_t Landmarks::GROUP_ADDED_SIG      = "groupAdded";
const core::com::signals::key_t Landmarks::GROUP_REMOVED_SIG    = "groupMoved";
const core::com::signals::key_t Landmarks::POINT_ADDED_SIG      = "pointAdded";
const core::com::signals::key_t Landmarks::POINT_REMOVED_SIG    = "pointRemoved";
const core::com::signals::key_t Landmarks::POINT_INSERTED_SIG   = "pointInserted";
const core::com::signals::key_t Landmarks::GROUP_MODIFIED_SIG   = "groupModified";
const core::com::signals::key_t Landmarks::POINT_MODIFIED_SIG   = "pointModified";
const core::com::signals::key_t Landmarks::GROUP_RENAMED_SIG    = "groupRenamed";
const core::com::signals::key_t Landmarks::POINT_SELECTED_SIG   = "pointSelected";
const core::com::signals::key_t Landmarks::POINT_DESELECTED_SIG = "pointDeselected";

//------------------------------------------------------------------------------

Landmarks::Landmarks()
{
    new_signal<GroupAddedSignalType>(GROUP_ADDED_SIG);
    new_signal<GroupRemovedSignalType>(GROUP_REMOVED_SIG);
    new_signal<PointAddedSignalType>(POINT_ADDED_SIG);
    new_signal<PointRemovedSignalType>(POINT_REMOVED_SIG);
    new_signal<PointInsertedSignalType>(POINT_INSERTED_SIG);
    new_signal<GroupModifiedSignalType>(GROUP_MODIFIED_SIG);
    new_signal<PointModifiedSigType>(POINT_MODIFIED_SIG);
    new_signal<GroupRenamedSignalType>(GROUP_RENAMED_SIG);
    new_signal<PointSelectedSignalType>(POINT_SELECTED_SIG);
    new_signal<PointDeselectedSignalType>(POINT_DESELECTED_SIG);
}

//------------------------------------------------------------------------------

void Landmarks::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const Landmarks>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_landmarks = other->m_landmarks;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void Landmarks::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const Landmarks>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_landmarks = other->m_landmarks;

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void Landmarks::addGroup(
    const std::string& name,
    const Landmarks::ColorType& color,
    const Landmarks::SizeType size,
    const Landmarks::Shape shape,
    const bool visibility
)
{
    LandmarksGroup group(color, size, shape, visibility);
    const auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Group '" + name + "' already exists"), iter != m_landmarks.end());
    m_landmarks.insert(std::make_pair(name, group));
}

//------------------------------------------------------------------------------

Landmarks::GroupNameContainer Landmarks::getGroupNames() const
{
    Landmarks::GroupNameContainer names;

    std::transform(
        m_landmarks.begin(),
        m_landmarks.end(),
        std::back_inserter(names),
        [](const LandmarksContainer::value_type& pair){return pair.first;});

    return names;
}

//------------------------------------------------------------------------------

const Landmarks::LandmarksGroup& Landmarks::getGroup(const std::string& name) const
{
    const auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Group '" + name + "' does not exist"), iter == m_landmarks.end());
    const Landmarks::LandmarksGroup& group = iter->second;
    return group;
}

//------------------------------------------------------------------------------

Landmarks::LandmarksGroup& Landmarks::getGroup(const std::string& name)
{
    auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Group '" + name + "' does not exist"), iter == m_landmarks.end());

    Landmarks::LandmarksGroup& group = iter->second;
    return group;
}

//------------------------------------------------------------------------------

bool Landmarks::hasGroup(const std::string& name) const noexcept
{
    return m_landmarks.contains(name);
}

//------------------------------------------------------------------------------

void Landmarks::renameGroup(const std::string& oldName, const std::string& newName)
{
    const auto iter = m_landmarks.find(oldName);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Group '" + oldName + "' does not exist"), iter == m_landmarks.end());
    const auto iter2 = m_landmarks.find(newName);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Group '" + newName + "' already exists"), iter2 != m_landmarks.end());

    const Landmarks::LandmarksGroup group = iter->second;
    m_landmarks.insert(std::make_pair(newName, group));
    m_landmarks.erase(oldName);
}

//------------------------------------------------------------------------------

void Landmarks::removeGroup(const std::string& name)
{
    const auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Group '" + name + "' does not exist"), iter == m_landmarks.end());

    m_landmarks.erase(name);
}

//------------------------------------------------------------------------------

void Landmarks::setGroupColor(const std::string& name, const Landmarks::ColorType& color)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_color = color;
}

//------------------------------------------------------------------------------

void Landmarks::setGroupSize(const std::string& name, const Landmarks::SizeType size)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_size = size;
}

//------------------------------------------------------------------------------

void Landmarks::setGroupShape(const std::string& name, const Landmarks::Shape shape)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_shape = shape;
}

//------------------------------------------------------------------------------

void Landmarks::setGroupVisibility(const std::string& name, const bool visibility)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_visibility = visibility;
}

//------------------------------------------------------------------------------

void Landmarks::addPoint(const std::string& name, const Landmarks::PointType& point)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_points.push_back(point);
}

//------------------------------------------------------------------------------

void Landmarks::insertPoint(const std::string& name, const std::size_t index, const Landmarks::PointType& point)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    auto iter                        = group.m_points.begin() + static_cast<PointContainer::difference_type>(index);
    group.m_points.insert(iter, point);
}

//------------------------------------------------------------------------------

const Landmarks::PointType& Landmarks::getPoint(const std::string& name, std::size_t index) const
{
    const Landmarks::LandmarksGroup& group = this->getGroup(name);
    return group.m_points.at(index);
}

//------------------------------------------------------------------------------

Landmarks::PointType& Landmarks::getPoint(const std::string& name, std::size_t index)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    return group.m_points.at(index);
}

//------------------------------------------------------------------------------

const Landmarks::PointContainer& Landmarks::getPoints(const std::string& name) const
{
    const Landmarks::LandmarksGroup& group = this->getGroup(name);
    return group.m_points;
}

//------------------------------------------------------------------------------

void Landmarks::removePoint(const std::string& name, std::size_t index)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);

    SIGHT_THROW_EXCEPTION_IF(
        std::out_of_range("index out of range in group '" + name + "'"),
        index >= group.m_points.size()
    );

    auto iter = group.m_points.begin() + static_cast<PointContainer::difference_type>(index);
    group.m_points.erase(iter);
}

//------------------------------------------------------------------------------

void Landmarks::clearPoints(const std::string& name)
{
    Landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_points.clear();
}

//------------------------------------------------------------------------------

std::size_t Landmarks::numPoints() const
{
    std::size_t nb = 0;
    for(const auto& elt : m_landmarks)
    {
        const LandmarksGroup group = elt.second;
        nb += group.m_points.size();
    }

    return nb;
}

//------------------------------------------------------------------------------

std::size_t Landmarks::numPoints(const std::string& name) const
{
    const LandmarksGroup& group = this->getGroup(name);
    return group.m_points.size();
}

//------------------------------------------------------------------------------

bool Landmarks::LandmarksGroup::operator==(const LandmarksGroup& other) const noexcept
{
    return !(!core::tools::is_equal(m_color, other.m_color)
             || !core::tools::is_equal(m_size, other.m_size)
             || m_shape != other.m_shape
             || m_visibility != other.m_visibility
             || !core::tools::is_equal(m_points, other.m_points));
}

//------------------------------------------------------------------------------

bool Landmarks::LandmarksGroup::operator!=(const LandmarksGroup& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool Landmarks::operator==(const Landmarks& other) const noexcept
{
    if(!core::tools::is_equal(m_landmarks, other.m_landmarks))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool Landmarks::operator!=(const Landmarks& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
