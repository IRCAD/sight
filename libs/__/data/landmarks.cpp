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

#include "data/landmarks.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::landmarks);

namespace sight::data
{

const core::com::signals::key_t landmarks::GROUP_ADDED_SIG      = "groupAdded";
const core::com::signals::key_t landmarks::GROUP_REMOVED_SIG    = "groupMoved";
const core::com::signals::key_t landmarks::POINT_ADDED_SIG      = "pointAdded";
const core::com::signals::key_t landmarks::POINT_REMOVED_SIG    = "pointRemoved";
const core::com::signals::key_t landmarks::POINT_INSERTED_SIG   = "pointInserted";
const core::com::signals::key_t landmarks::GROUP_MODIFIED_SIG   = "groupModified";
const core::com::signals::key_t landmarks::POINT_MODIFIED_SIG   = "pointModified";
const core::com::signals::key_t landmarks::GROUP_RENAMED_SIG    = "groupRenamed";
const core::com::signals::key_t landmarks::POINT_SELECTED_SIG   = "pointSelected";
const core::com::signals::key_t landmarks::POINT_DESELECTED_SIG = "pointDeselected";

//------------------------------------------------------------------------------

landmarks::landmarks()
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

void landmarks::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const landmarks>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_landmarks = other->m_landmarks;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void landmarks::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const landmarks>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_landmarks = other->m_landmarks;

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void landmarks::addGroup(
    const std::string& name,
    const landmarks::ColorType& color,
    const landmarks::SizeType size,
    const landmarks::Shape shape,
    const bool visibility
)
{
    LandmarksGroup group(color, size, shape, visibility);
    const auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + name + "' already exists"), iter != m_landmarks.end());
    m_landmarks.insert(std::make_pair(name, group));
}

//------------------------------------------------------------------------------

landmarks::GroupNameContainer landmarks::getGroupNames() const
{
    landmarks::GroupNameContainer names;

    std::transform(
        m_landmarks.begin(),
        m_landmarks.end(),
        std::back_inserter(names),
        [](const LandmarksContainer::value_type& pair){return pair.first;});

    return names;
}

//------------------------------------------------------------------------------

const landmarks::LandmarksGroup& landmarks::getGroup(const std::string& name) const
{
    const auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + name + "' does not exist"), iter == m_landmarks.end());
    const landmarks::LandmarksGroup& group = iter->second;
    return group;
}

//------------------------------------------------------------------------------

landmarks::LandmarksGroup& landmarks::getGroup(const std::string& name)
{
    auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + name + "' does not exist"), iter == m_landmarks.end());

    landmarks::LandmarksGroup& group = iter->second;
    return group;
}

//------------------------------------------------------------------------------

bool landmarks::hasGroup(const std::string& name) const noexcept
{
    return m_landmarks.contains(name);
}

//------------------------------------------------------------------------------

void landmarks::renameGroup(const std::string& oldName, const std::string& newName)
{
    const auto iter = m_landmarks.find(oldName);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + oldName + "' does not exist"), iter == m_landmarks.end());
    const auto iter2 = m_landmarks.find(newName);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + newName + "' already exists"), iter2 != m_landmarks.end());

    const landmarks::LandmarksGroup group = iter->second;
    m_landmarks.insert(std::make_pair(newName, group));
    m_landmarks.erase(oldName);
}

//------------------------------------------------------------------------------

void landmarks::removeGroup(const std::string& name)
{
    const auto iter = m_landmarks.find(name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + name + "' does not exist"), iter == m_landmarks.end());

    m_landmarks.erase(name);
}

//------------------------------------------------------------------------------

void landmarks::setGroupColor(const std::string& name, const landmarks::ColorType& color)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_color = color;
}

//------------------------------------------------------------------------------

void landmarks::setGroupSize(const std::string& name, const landmarks::SizeType size)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_size = size;
}

//------------------------------------------------------------------------------

void landmarks::setGroupShape(const std::string& name, const landmarks::Shape shape)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_shape = shape;
}

//------------------------------------------------------------------------------

void landmarks::setGroupVisibility(const std::string& name, const bool visibility)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_visibility = visibility;
}

//------------------------------------------------------------------------------

void landmarks::addPoint(const std::string& name, const landmarks::PointType& point)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_points.push_back(point);
}

//------------------------------------------------------------------------------

void landmarks::insertPoint(const std::string& name, const std::size_t index, const landmarks::PointType& point)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    auto iter                        = group.m_points.begin() + static_cast<PointContainer::difference_type>(index);
    group.m_points.insert(iter, point);
}

//------------------------------------------------------------------------------

const landmarks::PointType& landmarks::getPoint(const std::string& name, std::size_t index) const
{
    const landmarks::LandmarksGroup& group = this->getGroup(name);
    return group.m_points.at(index);
}

//------------------------------------------------------------------------------

landmarks::PointType& landmarks::getPoint(const std::string& name, std::size_t index)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    return group.m_points.at(index);
}

//------------------------------------------------------------------------------

const landmarks::PointContainer& landmarks::getPoints(const std::string& name) const
{
    const landmarks::LandmarksGroup& group = this->getGroup(name);
    return group.m_points;
}

//------------------------------------------------------------------------------

void landmarks::removePoint(const std::string& name, std::size_t index)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);

    SIGHT_THROW_EXCEPTION_IF(
        std::out_of_range("index out of range in group '" + name + "'"),
        index >= group.m_points.size()
    );

    auto iter = group.m_points.begin() + static_cast<PointContainer::difference_type>(index);
    group.m_points.erase(iter);
}

//------------------------------------------------------------------------------

void landmarks::clearPoints(const std::string& name)
{
    landmarks::LandmarksGroup& group = this->getGroup(name);
    group.m_points.clear();
}

//------------------------------------------------------------------------------

std::size_t landmarks::numPoints() const
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

std::size_t landmarks::numPoints(const std::string& name) const
{
    const LandmarksGroup& group = this->getGroup(name);
    return group.m_points.size();
}

//------------------------------------------------------------------------------

bool landmarks::LandmarksGroup::operator==(const LandmarksGroup& other) const noexcept
{
    return !(!core::tools::is_equal(m_color, other.m_color)
             || !core::tools::is_equal(m_size, other.m_size)
             || m_shape != other.m_shape
             || m_visibility != other.m_visibility
             || !core::tools::is_equal(m_points, other.m_points));
}

//------------------------------------------------------------------------------

bool landmarks::LandmarksGroup::operator!=(const LandmarksGroup& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

bool landmarks::operator==(const landmarks& other) const noexcept
{
    if(!core::tools::is_equal(m_landmarks, other.m_landmarks))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool landmarks::operator!=(const landmarks& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
