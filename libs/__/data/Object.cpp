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

#include "data/Object.hpp"

#include "data/factory/new.hpp"

#include <core/com/signal.hxx>

#include <functional>

namespace sight::data
{

//------------------------------------------------------------------------------

const core::com::signals::key_t Object::MODIFIED_SIG       = "modified";
const core::com::signals::key_t Object::ADDED_FIELDS_SIG   = "addedFields";
const core::com::signals::key_t Object::CHANGED_FIELDS_SIG = "changedFields";
const core::com::signals::key_t Object::REMOVED_FIELDS_SIG = "removedFields";

//------------------------------------------------------------------------------

Object::Object()
{
    new_signal<ModifiedSignalType>(MODIFIED_SIG);
    new_signal<AddedFieldsSignalType>(ADDED_FIELDS_SIG);
    new_signal<ChangedFieldsSignalType>(CHANGED_FIELDS_SIG);
    new_signal<RemovedFieldsSignalType>(REMOVED_FIELDS_SIG);
}

//------------------------------------------------------------------------------

data::Object::sptr Object::getField(const FieldNameType& name, data::Object::sptr defaultValue) const
{
    data::Object::sptr object = defaultValue;
    auto iter                 = m_fields.find(name);
    if(iter != m_fields.end())
    {
        object = iter->second;
    }

    return object;
}

//------------------------------------------------------------------------------

data::Object::csptr Object::getConstField(const FieldNameType& name) const
{
    return this->getField(name);
}

//------------------------------------------------------------------------------

const Object::FieldMapType& Object::getFields() const
{
    return m_fields;
}

//------------------------------------------------------------------------------

Object::FieldNameVectorType Object::getFieldNames() const
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

void Object::setField(const FieldNameType& name, data::Object::sptr obj)
{
    std::pair<FieldMapType::iterator, bool> res = m_fields.insert(FieldMapType::value_type(name, obj));
    if(!res.second)
    {
        res.first->second = obj;
    }
}

//------------------------------------------------------------------------------

void Object::setFields(const FieldMapType& fieldMap)
{
    m_fields = fieldMap;
}

//------------------------------------------------------------------------------

void Object::removeField(const FieldNameType& name)
{
    auto iter = m_fields.find(name);
    SIGHT_ASSERT("Field " << name << " not found.", iter != m_fields.end());
    if(iter != m_fields.end())
    {
        m_fields.erase(iter);
    }
}

//------------------------------------------------------------------------------

void Object::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    SIGHT_ASSERT("Cache shall not be null !", cache);

    // Copy members
    m_description = source->m_description;

    // Deep copy fields
    m_fields.clear();

    for(const auto& [key, value] : source->m_fields)
    {
        m_fields.insert_or_assign(key, Object::copy(value, cache));
    }
}

//-----------------------------------------------------------------------------

void Object::shallow_copy(const Object::csptr& source)
{
    // Copy members
    m_description = source->m_description;

    // Shallow copy fields
    m_fields = source->m_fields;
}

//-----------------------------------------------------------------------------

data::Object::sptr Object::copy(const data::Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
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

bool Object::operator==(const Object& other) const noexcept
{
    if(m_description != other.m_description)
    {
        return false;
    }

    return core::tools::is_equal(m_fields, other.m_fields);
}

//------------------------------------------------------------------------------

bool Object::operator!=(const Object& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
