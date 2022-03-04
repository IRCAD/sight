/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

//FIXME :This needs to be include first for GCC
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
//
#include "data/factory/new.hpp"
#include "data/Object.hpp"

#include <functional>

namespace sight::data
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType Object::s_MODIFIED_SIG       = "modified";
const core::com::Signals::SignalKeyType Object::s_ADDED_FIELDS_SIG   = "addedFields";
const core::com::Signals::SignalKeyType Object::s_CHANGED_FIELDS_SIG = "changedFields";
const core::com::Signals::SignalKeyType Object::s_REMOVED_FIELDS_SIG = "removedFields";

//------------------------------------------------------------------------------

Object::Object()
{
    newSignal<ModifiedSignalType>(s_MODIFIED_SIG);
    newSignal<AddedFieldsSignalType>(s_ADDED_FIELDS_SIG);
    newSignal<ChangedFieldsSignalType>(s_CHANGED_FIELDS_SIG);
    newSignal<RemovedFieldsSignalType>(s_REMOVED_FIELDS_SIG);
}

//------------------------------------------------------------------------------

Object::~Object()
{
}

//------------------------------------------------------------------------------

data::Object::sptr Object::getField(const FieldNameType& name, data::Object::sptr defaultValue) const
{
    data::Object::sptr object         = defaultValue;
    FieldMapType::const_iterator iter = m_fields.find(name);
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
        std::bind(&FieldMapType::value_type::first, std::placeholders::_1)
    );
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
    FieldMapType::const_iterator iter = m_fields.find(name);
    SIGHT_ASSERT("Field " << name << " not found.", iter != m_fields.end());
    if(iter != m_fields.end())
    {
        m_fields.erase(iter);
    }
}

//-----------------------------------------------------------------------------

void Object::fieldShallowCopy(const data::Object::csptr& source)
{
    this->setFields(source->getFields());
}

//-----------------------------------------------------------------------------

void Object::deepCopy(const data::Object::csptr& source)
{
    DeepCopyCacheType cache;
    return this->cachedDeepCopy(source, cache);
}

//-----------------------------------------------------------------------------

void Object::fieldDeepCopy(const data::Object::csptr& source)
{
    DeepCopyCacheType cache;
    return this->fieldDeepCopy(source, cache);
}

//-----------------------------------------------------------------------------

void Object::fieldDeepCopy(const data::Object::csptr& source, DeepCopyCacheType& cache)
{
    m_fields.clear();
    const data::Object::FieldMapType& sourceFields = source->getFields();
    for(const data::Object::FieldMapType::value_type& elt : sourceFields)
    {
        this->setField(elt.first, data::Object::copy(elt.second, cache));
    }
}

//-----------------------------------------------------------------------------

void Object::shallowCopy(const data::Object::csptr& source)
{
    SIGHT_NOT_USED(source);
    SIGHT_FATAL("shallowCopy not implemented for : " + this->getClassname());
}

//-----------------------------------------------------------------------------

data::Object::sptr Object::copy(const data::Object::csptr& source)
{
    DeepCopyCacheType cache;
    return Object::copy(source, cache);
}

//-----------------------------------------------------------------------------

data::Object::sptr Object::copy(const data::Object::csptr& source, Object::DeepCopyCacheType& cache)
{
    data::Object::sptr obj;

    if(source)
    {
        DeepCopyCacheType::const_iterator cacheItem = cache.find(source);

        if(cacheItem == cache.end())
        {
            obj = data::factory::New(source->getClassname());
            SIGHT_ASSERT("Could not create object of type : " + source->getClassname(), obj);
            cache.insert(DeepCopyCacheType::value_type(source, obj));
            obj->cachedDeepCopy(source, cache);
        }
        else
        {
            obj = cacheItem->second;
        }
    }

    return obj;
}

//-----------------------------------------------------------------------------

bool Object::operator==(const Object& other) const noexcept
{
    if(!core::tools::is_equal(m_fields, other.m_fields))
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

bool Object::operator!=(const Object& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
