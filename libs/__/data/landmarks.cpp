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
    new_signal<group_added_signal_t>(GROUP_ADDED_SIG);
    new_signal<group_removed_signal_t>(GROUP_REMOVED_SIG);
    new_signal<point_added_signal_t>(POINT_ADDED_SIG);
    new_signal<point_removed_signal_t>(POINT_REMOVED_SIG);
    new_signal<point_inserted_signal_t>(POINT_INSERTED_SIG);
    new_signal<group_modified_signal_t>(GROUP_MODIFIED_SIG);
    new_signal<point_modified_sig_t>(POINT_MODIFIED_SIG);
    new_signal<group_renamed_signal_t>(GROUP_RENAMED_SIG);
    new_signal<point_selected_signal_t>(POINT_SELECTED_SIG);
    new_signal<point_deselected_signal_t>(POINT_DESELECTED_SIG);
}

//------------------------------------------------------------------------------

void landmarks::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const landmarks>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_landmarks = other->m_landmarks;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void landmarks::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const landmarks>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_landmarks = other->m_landmarks;

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

void landmarks::add_group(
    const std::string& _name,
    const landmarks::color_t& _color,
    const landmarks::size_t _size,
    const landmarks::shape _shape,
    const bool _visibility
)
{
    landmarks_group group(_color, _size, _shape, _visibility);
    const auto iter = m_landmarks.find(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + _name + "' already exists"), iter != m_landmarks.end());
    m_landmarks.insert(std::make_pair(_name, group));
}

//------------------------------------------------------------------------------

landmarks::GroupNameContainer landmarks::get_group_names() const
{
    landmarks::GroupNameContainer names;

    std::transform(
        m_landmarks.begin(),
        m_landmarks.end(),
        std::back_inserter(names),
        [](const landmarks_container::value_type& _pair){return _pair.first;});

    return names;
}

//------------------------------------------------------------------------------

const landmarks::landmarks_group& landmarks::get_group(const std::string& _name) const
{
    const auto iter = m_landmarks.find(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + _name + "' does not exist"), iter == m_landmarks.end());
    const landmarks::landmarks_group& group = iter->second;
    return group;
}

//------------------------------------------------------------------------------

landmarks::landmarks_group& landmarks::get_group(const std::string& _name)
{
    auto iter = m_landmarks.find(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + _name + "' does not exist"), iter == m_landmarks.end());

    landmarks::landmarks_group& group = iter->second;
    return group;
}

//------------------------------------------------------------------------------

bool landmarks::has_group(const std::string& _name) const noexcept
{
    return m_landmarks.contains(_name);
}

//------------------------------------------------------------------------------

void landmarks::rename_group(const std::string& _old_name, const std::string& _new_name)
{
    const auto iter = m_landmarks.find(_old_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + _old_name + "' does not exist"), iter == m_landmarks.end());
    const auto iter2 = m_landmarks.find(_new_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + _new_name + "' already exists"), iter2 != m_landmarks.end());

    const landmarks::landmarks_group group = iter->second;
    m_landmarks.insert(std::make_pair(_new_name, group));
    m_landmarks.erase(_old_name);
}

//------------------------------------------------------------------------------

void landmarks::remove_group(const std::string& _name)
{
    const auto iter = m_landmarks.find(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Group '" + _name + "' does not exist"), iter == m_landmarks.end());

    m_landmarks.erase(_name);
}

//------------------------------------------------------------------------------

void landmarks::set_group_color(const std::string& _name, const landmarks::color_t& _color)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    group.m_color = _color;
}

//------------------------------------------------------------------------------

void landmarks::set_group_size(const std::string& _name, const landmarks::size_t _size)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    group.m_size = _size;
}

//------------------------------------------------------------------------------

void landmarks::set_group_shape(const std::string& _name, const landmarks::shape _shape)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    group.m_shape = _shape;
}

//------------------------------------------------------------------------------

void landmarks::set_group_visibility(const std::string& _name, const bool _visibility)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    group.m_visibility = _visibility;
}

//------------------------------------------------------------------------------

void landmarks::add_point(const std::string& _name, const landmarks::point_t& _point)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    group.m_points.push_back(_point);
}

//------------------------------------------------------------------------------

void landmarks::insert_point(const std::string& _name, const std::size_t _index, const landmarks::point_t& _point)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    auto iter                         = group.m_points.begin() + static_cast<point_container::difference_type>(_index);
    group.m_points.insert(iter, _point);
}

//------------------------------------------------------------------------------

const landmarks::point_t& landmarks::get_point(const std::string& _name, std::size_t _index) const
{
    const landmarks::landmarks_group& group = this->get_group(_name);
    return group.m_points.at(_index);
}

//------------------------------------------------------------------------------

landmarks::point_t& landmarks::get_point(const std::string& _name, std::size_t _index)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    return group.m_points.at(_index);
}

//------------------------------------------------------------------------------

const landmarks::point_container& landmarks::get_points(const std::string& _name) const
{
    const landmarks::landmarks_group& group = this->get_group(_name);
    return group.m_points;
}

//------------------------------------------------------------------------------

void landmarks::remove_point(const std::string& _name, std::size_t _index)
{
    landmarks::landmarks_group& group = this->get_group(_name);

    SIGHT_THROW_EXCEPTION_IF(
        std::out_of_range("index out of range in group '" + _name + "'"),
        _index >= group.m_points.size()
    );

    auto iter = group.m_points.begin() + static_cast<point_container::difference_type>(_index);
    group.m_points.erase(iter);
}

//------------------------------------------------------------------------------

void landmarks::clear_points(const std::string& _name)
{
    landmarks::landmarks_group& group = this->get_group(_name);
    group.m_points.clear();
}

//------------------------------------------------------------------------------

std::size_t landmarks::num_points() const
{
    std::size_t nb = 0;
    for(const auto& elt : m_landmarks)
    {
        const landmarks_group group = elt.second;
        nb += group.m_points.size();
    }

    return nb;
}

//------------------------------------------------------------------------------

std::size_t landmarks::num_points(const std::string& _name) const
{
    const landmarks_group& group = this->get_group(_name);
    return group.m_points.size();
}

//------------------------------------------------------------------------------

bool landmarks::landmarks_group::operator==(const landmarks_group& _other) const noexcept
{
    return !(!core::tools::is_equal(m_color, _other.m_color)
             || !core::tools::is_equal(m_size, _other.m_size)
             || m_shape != _other.m_shape
             || m_visibility != _other.m_visibility
             || !core::tools::is_equal(m_points, _other.m_points));
}

//------------------------------------------------------------------------------

bool landmarks::landmarks_group::operator!=(const landmarks_group& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

bool landmarks::operator==(const landmarks& _other) const noexcept
{
    if(!core::tools::is_equal(m_landmarks, _other.m_landmarks))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool landmarks::operator!=(const landmarks& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
