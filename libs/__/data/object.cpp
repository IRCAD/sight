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
    new_signal<ModifiedSignalType>(MODIFIED_SIG);
    new_signal<AddedFieldsSignalType>(ADDED_FIELDS_SIG);
    new_signal<ChangedFieldsSignalType>(CHANGED_FIELDS_SIG);
    new_signal<RemovedFieldsSignalType>(REMOVED_FIELDS_SIG);
}

//------------------------------------------------------------------------------

data::object::sptr object::getField(const FieldNameType& name, data::object::sptr defaultValue) const
{
    data::object::sptr object = defaultValue;
    auto iter                 = m_fields.find(name);
    if(iter != m_fields.end())
    {
        object = iter->second;
    }

    return object;
}

//------------------------------------------------------------------------------

data::object::csptr object::getConstField(const FieldNameType& name) const
{
    return this->getField(name);
}

//------------------------------------------------------------------------------

const object::FieldMapType& object::getFields() const
{
    return m_fields;
}

//------------------------------------------------------------------------------

object::FieldNameVectorType object::getFieldNames() const
{
    FieldNameVectorType names;
    std::transform(
        m_fields.begin(),
        m_fields.end(),
        std::back_inserter(names),
        [](const auto& e){return e.first;});
    return names;
}

//------------------------------------------------------------------------------

void object::setField(const FieldNameType& name, data::object::sptr obj)
{
    std::pair<FieldMapType::iterator, bool> res = m_fields.insert(FieldMapType::value_type(name, obj));
    if(!res.second)
    {
        res.first->second = obj;
    }
}

//------------------------------------------------------------------------------

void object::setFields(const FieldMapType& fieldMap)
{
    m_fields = fieldMap;
}

//------------------------------------------------------------------------------

void object::removeField(const FieldNameType& name)
{
    auto iter = m_fields.find(name);
    SIGHT_ASSERT("Field " << name << " not found.", iter != m_fields.end());
    if(iter != m_fields.end())
    {
        m_fields.erase(iter);
    }
}

//------------------------------------------------------------------------------

void object::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    SIGHT_ASSERT("Cache shall not be null !", cache);

    // Copy members
    m_description = source->m_description;

    // Deep copy fields
    m_fields.clear();

    for(const auto& [key, value] : source->m_fields)
    {
        m_fields.insert_or_assign(key, object::copy(value, cache));
    }
}

//-----------------------------------------------------------------------------

void object::shallow_copy(const object::csptr& source)
{
    // Copy members
    m_description = source->m_description;

    // Shallow copy fields
    m_fields = source->m_fields;
}

//-----------------------------------------------------------------------------

data::object::sptr object::copy(const data::object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    SIGHT_ASSERT("Cache shall not be null !", cache);

    if(!source)
    {
        return nullptr;
    }

    // Check if the object is already in the cache
    if(const auto& it = cache->find(source); it != cache->cend())
    {
        return it->second;
    }

    // Not found in cache, we crete a new copy
    auto object_copy = data::factory::make(source->get_classname());
    SIGHT_ASSERT("Could not create object of type : " + source->get_classname(), object_copy);

    // Add the object to the cache
    cache->insert_or_assign(source, object_copy);

    // Copy the object
    object_copy->deep_copy(source, cache);

    return object_copy;
}

//-----------------------------------------------------------------------------

bool object::operator==(const object& other) const noexcept
{
    if(m_description != other.m_description)
    {
        return false;
    }

    return core::tools::is_equal(m_fields, other.m_fields);
}

//------------------------------------------------------------------------------

bool object::operator!=(const object& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
