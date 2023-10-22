/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/object.hpp"

#include "data/factory/new.hpp"

#include <core/com/signal.hxx>

#include <functional>

namespace sight::data
{

//------------------------------------------------------------------------------

const core::com::signals::key_t object::MODIFIED_SIG       = "modified";
const core::com::signals::key_t object::ADDED_FIELDS_SIG   = "addedFields";
const core::com::signals::key_t object::CHANGED_FIELDS_SIG = "changedFields";
const core::com::signals::key_t object::REMOVED_FIELDS_SIG = "removedFields";

//------------------------------------------------------------------------------

object::object()
{
    new_signal<modified_signal_t>(MODIFIED_SIG);
    new_signal<added_fields_signal_t>(ADDED_FIELDS_SIG);
    new_signal<changed_fields_signal_t>(CHANGED_FIELDS_SIG);
    new_signal<removed_fields_signal_t>(REMOVED_FIELDS_SIG);
}

//------------------------------------------------------------------------------

data::object::sptr object::get_field(const field_name_t& _name, data::object::sptr _default_value) const
{
    data::object::sptr object = _default_value;
    auto iter                 = m_fields.find(_name);
    if(iter != m_fields.end())
    {
        object = iter->second;
    }

    return object;
}

//------------------------------------------------------------------------------

const object::field_map_t& object::get_fields() const
{
    return m_fields;
}

//------------------------------------------------------------------------------

object::field_name_vector_t object::get_field_names() const
{
    field_name_vector_t names;
    std::transform(
        m_fields.begin(),
        m_fields.end(),
        std::back_inserter(names),
        [](const auto& _e){return _e.first;});
    return names;
}

//------------------------------------------------------------------------------

void object::set_field(const field_name_t& _name, data::object::sptr _obj)
{
    std::pair<field_map_t::iterator, bool> res = m_fields.insert(field_map_t::value_type(_name, _obj));
    if(!res.second)
    {
        res.first->second = _obj;
    }
}

//------------------------------------------------------------------------------

void object::set_fields(const field_map_t& _field_map)
{
    m_fields = _field_map;
}

//------------------------------------------------------------------------------

void object::remove_field(const field_name_t& _name)
{
    auto iter = m_fields.find(_name);
    SIGHT_ASSERT("field " << _name << " not found.", iter != m_fields.end());
    if(iter != m_fields.end())
    {
        m_fields.erase(iter);
    }
}

//------------------------------------------------------------------------------

void object::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    SIGHT_ASSERT("Cache shall not be null !", _cache);

    // Copy members
    m_description = _source->m_description;

    // Deep copy fields
    m_fields.clear();

    for(const auto& [key, value] : _source->m_fields)
    {
        m_fields.insert_or_assign(key, object::copy(value, _cache));
    }
}

//-----------------------------------------------------------------------------

void object::shallow_copy(const object::csptr& _source)
{
    // Copy members
    m_description = _source->m_description;

    // Shallow copy fields
    m_fields = _source->m_fields;
}

//-----------------------------------------------------------------------------

data::object::sptr object::copy(const data::object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    SIGHT_ASSERT("Cache shall not be null !", _cache);

    if(!_source)
    {
        return nullptr;
    }

    // Check if the object is already in the cache
    if(const auto& it = _cache->find(_source); it != _cache->cend())
    {
        return it->second;
    }

    // Not found in cache, we crete a new copy
    auto object_copy = data::factory::make(_source->get_classname());
    SIGHT_ASSERT("Could not create object of type : " + _source->get_classname(), object_copy);

    // Add the object to the cache
    _cache->insert_or_assign(_source, object_copy);

    // Copy the object
    object_copy->deep_copy(_source, _cache);

    return object_copy;
}

//-----------------------------------------------------------------------------

bool object::operator==(const object& _other) const noexcept
{
    if(m_description != _other.m_description)
    {
        return false;
    }

    return core::tools::is_equal(m_fields, _other.m_fields);
}

//------------------------------------------------------------------------------

bool object::operator!=(const object& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
