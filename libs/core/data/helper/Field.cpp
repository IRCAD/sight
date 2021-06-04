/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/helper/Field.hpp"

#include <data/Exception.hpp>
#include <data/Object.hpp>

#include <core/com/Signal.hxx>

#include <algorithm>
#include <functional>

namespace sight::data
{

namespace helper
{

//-----------------------------------------------------------------------------

Field::Field(data::Object::sptr object) :
    m_object(object)
{
}

//-----------------------------------------------------------------------------

Field::~Field()
{
    if(!m_addedFields.empty() || !m_newChangedFields.empty() || !m_removedFields.empty())
    {
        notify();
    }
}

//-----------------------------------------------------------------------------

void Field::setField(const data::Object::FieldNameType& name, data::Object::sptr obj)
{
    this->addOrSwap(name, obj);
}

//-----------------------------------------------------------------------------

void Field::setFields(const data::Object::FieldMapType& newFields)
{
    data::Object::sptr object = m_object.lock();
    SIGHT_ASSERT("Field helper need a non-null object pointer", object);
    const data::Object::FieldMapType oldFields = object->getFields();
    this->buildMessage(oldFields, newFields);
    object->setFields(newFields);
}

//-----------------------------------------------------------------------------

void Field::removeField(const data::Object::FieldNameType& name)
{
    this->remove(name);
}

//-----------------------------------------------------------------------------

void Field::add(const data::Object::FieldNameType& _name, data::Object::sptr _obj)
{
    data::Object::sptr object = m_object.lock();
    SIGHT_ASSERT("Field helper need a non-null object pointer", object);

    data::Object::sptr field = object->getField(_name);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Field already exists"), field);

    m_addedFields[_name] = _obj;

    object->setField(_name, _obj);
}

//-----------------------------------------------------------------------------

void Field::swap(const data::Object::FieldNameType& _name, data::Object::sptr _obj)
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::Object::sptr object = m_object.lock();

    data::Object::sptr field = object->getField(_name);
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Field does not exist"), !field);

    m_newChangedFields[_name] = _obj;
    m_oldChangedFields[_name] = field;

    object->setField(_name, _obj);
}

//-----------------------------------------------------------------------------

void Field::addOrSwap(const data::Object::FieldNameType& _name, data::Object::sptr _obj)
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::Object::sptr object = m_object.lock();

    data::Object::sptr field = object->getField(_name);

    if(!field)
    {
        m_addedFields[_name] = _obj;
    }
    else
    {
        m_newChangedFields[_name] = _obj;
        m_oldChangedFields[_name] = field;
    }

    object->setField(_name, _obj);
}

//-----------------------------------------------------------------------------

void Field::remove(const data::Object::FieldNameType& _name)
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::Object::sptr object = m_object.lock();
    data::Object::sptr field  = object->getField(_name);

    SIGHT_THROW_EXCEPTION_IF(data::Exception("Field does not exist"), !field);

    m_removedFields[_name] = field;
    object->removeField(_name);
}

//-----------------------------------------------------------------------------

void Field::clear()
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::Object::sptr object = m_object.lock();

    auto fieldNames = object->getFieldNames();
    for(const auto& name : fieldNames)
    {
        data::Object::sptr field = object->getField(name);
        m_removedFields[name] = field;
        object->removeField(name);
    }
}

//-----------------------------------------------------------------------------

void Field::notify()
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());

    if(!m_removedFields.empty())
    {
        auto sig = m_object.lock()->signal<data::Object::RemovedFieldsSignalType>(
            data::Object::s_REMOVED_FIELDS_SIG
        );

        sig->asyncEmit(m_removedFields);
    }

    if(!m_newChangedFields.empty() && !m_oldChangedFields.empty())
    {
        auto sig = m_object.lock()->signal<data::Object::ChangedFieldsSignalType>(
            data::Object::s_CHANGED_FIELDS_SIG
        );

        sig->asyncEmit(m_newChangedFields, m_oldChangedFields);
    }

    if(!m_addedFields.empty())
    {
        auto sig = m_object.lock()->signal<data::Object::AddedFieldsSignalType>(
            data::Object::s_ADDED_FIELDS_SIG
        );

        sig->asyncEmit(m_addedFields);
    }

    SIGHT_INFO_IF(
        "No changes were found on the fields of the object '" + m_object.lock()->getID()
        + "', nothing to notify.",
        m_addedFields.empty() && m_newChangedFields.empty() && m_removedFields.empty()
    );

    m_removedFields.clear();
    m_newChangedFields.clear();
    m_oldChangedFields.clear();
    m_addedFields.clear();
}

//-----------------------------------------------------------------------------

void Field::buildMessage(
    const data::Object::FieldMapType& oldFields,
    const data::Object::FieldMapType& newFields
)
{
    data::Object::FieldNameVectorType oldFieldNames;
    data::Object::FieldNameVectorType newFieldNames;

    std::transform(
        oldFields.begin(),
        oldFields.end(),
        std::back_inserter(oldFieldNames),
        std::bind(&sight::data::Object::FieldMapType::value_type::first, std::placeholders::_1)
    );
    std::transform(
        newFields.begin(),
        newFields.end(),
        std::back_inserter(newFieldNames),
        std::bind(&sight::data::Object::FieldMapType::value_type::first, std::placeholders::_1)
    );

    std::sort(oldFieldNames.begin(), oldFieldNames.end());
    std::sort(newFieldNames.begin(), newFieldNames.end());

    data::Object::FieldNameVectorType added;   // new - old
    data::Object::FieldNameVectorType changed; // old & new
    data::Object::FieldNameVectorType removed; // old - new

    std::set_difference(
        newFieldNames.begin(),
        newFieldNames.end(),
        oldFieldNames.begin(),
        oldFieldNames.end(),
        std::back_inserter(added)
    );

    std::set_intersection(
        newFieldNames.begin(),
        newFieldNames.end(),
        oldFieldNames.begin(),
        oldFieldNames.end(),
        std::back_inserter(changed)
    );

    std::set_difference(
        oldFieldNames.begin(),
        oldFieldNames.end(),
        newFieldNames.begin(),
        newFieldNames.end(),
        std::back_inserter(removed)
    );

    for(const data::Object::FieldNameVectorType::value_type& fieldName : added)
    {
        m_addedFields[fieldName] = newFields.find(fieldName)->second;
    }

    for(const data::Object::FieldNameVectorType::value_type& fieldName : changed)
    {
        m_newChangedFields[fieldName] = newFields.find(fieldName)->second;
        m_oldChangedFields[fieldName] = oldFields.find(fieldName)->second;
    }

    for(const data::Object::FieldNameVectorType::value_type& fieldName : removed)
    {
        m_removedFields[fieldName] = oldFields.find(fieldName)->second;
    }
}

} // namespace helper

} // namespace sight::data
