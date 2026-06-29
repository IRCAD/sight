/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/resection.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::resection);

namespace sight::data
{

//------------------------------------------------------------------------------

resection::resection() :
    m_plane_list(std::make_shared<data::plane_list>())
{
    new_signal<signals::reconstruction_added_t>(signals::RECONSTRUCTION_ADDED);
    new_signal<signals::visibility_modified_t>(signals::VISIBILITY_MODIFIED);
}

//-----------------------------------------------------------------------------

void resection::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const resection>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name         = other->m_name;
    m_is_safe_part = other->m_is_safe_part;
    m_is_valid     = other->m_is_valid;
    m_visible      = other->m_visible;
    m_plane_list   = other->m_plane_list;
    m_v_inputs     = other->m_v_inputs;
    m_v_outputs    = other->m_v_outputs;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void resection::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const resection>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_name         = other->m_name;
    m_is_safe_part = other->m_is_safe_part;
    m_is_valid     = other->m_is_valid;
    m_visible      = other->m_visible;
    m_plane_list   = data::object::copy(other->m_plane_list, _cache);

    this->m_v_inputs.clear();
    for(const auto& r : other->m_v_inputs)
    {
        m_v_inputs.push_back(data::object::copy(r, _cache));
    }

    this->m_v_outputs.clear();
    for(const auto& r : other->m_v_outputs)
    {
        m_v_outputs.push_back(data::object::copy(r, _cache));
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool resection::operator==(const resection& _other) const noexcept
{
    if(m_name != _other.m_name
       || m_is_safe_part != _other.m_is_safe_part
       || m_is_valid != _other.m_is_valid
       || m_visible != _other.m_visible
       || !core::is_equal(m_plane_list, _other.m_plane_list)
       || !core::is_equal(m_v_inputs, _other.m_v_inputs)
       || !core::is_equal(m_v_outputs, _other.m_v_outputs))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool resection::operator!=(const resection& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
