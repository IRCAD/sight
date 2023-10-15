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

#include "data/helper/Field.hpp"

#include <core/com/signal.hxx>

#include <data/exception.hpp>
#include <data/object.hpp>

#include <algorithm>
#include <functional>

namespace sight::data::helper
{

//-----------------------------------------------------------------------------

Field::Field(data::object::sptr object) :
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

void Field::setField(const data::object::FieldNameType& name, data::object::sptr obj)
{
    this->addOrSwap(name, obj);
}

//-----------------------------------------------------------------------------

void Field::setFields(const data::object::FieldMapType& newFields)
{
    data::object::sptr object = m_object.lock();
    SIGHT_ASSERT("Field helper need a non-null object pointer", object);
    const data::object::FieldMapType oldFields = object->getFields();
    this->buildMessage(oldFields, newFields);
    object->setFields(newFields);
}

//-----------------------------------------------------------------------------

void Field::removeField(const data::object::FieldNameType& name)
{
    this->remove(name);
}

//-----------------------------------------------------------------------------

void Field::add(const data::object::FieldNameType& _name, data::object::sptr _obj)
{
    data::object::sptr object = m_object.lock();
    SIGHT_ASSERT("Field helper need a non-null object pointer", object);

    data::object::sptr field = object->getField(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Field already exists"), field);

    m_addedFields[_name] = _obj;

    object->setField(_name, _obj);
}

//-----------------------------------------------------------------------------

// NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap,performance-noexcept-swap)
void Field::swap(const data::object::FieldNameType& _name, data::object::sptr _obj)
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();

    data::object::sptr field = object->getField(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("Field does not exist"), !field);

    m_newChangedFields[_name] = _obj;
    m_oldChangedFields[_name] = field;

    object->setField(_name, _obj);
}

//-----------------------------------------------------------------------------

void Field::addOrSwap(const data::object::FieldNameType& _name, data::object::sptr _obj)
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();

    data::object::sptr field = object->getField(_name);

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

void Field::remove(const data::object::FieldNameType& _name)
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();
    data::object::sptr field  = object->getField(_name);

    SIGHT_THROW_EXCEPTION_IF(data::exception("Field does not exist"), !field);

    m_removedFields[_name] = field;
    object->removeField(_name);
}

//-----------------------------------------------------------------------------

void Field::clear()
{
    SIGHT_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();

    auto fieldNames = object->getFieldNames();
    for(const auto& name : fieldNames)
    {
        data::object::sptr field = object->getField(name);
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
        auto sig = m_object.lock()->signal<data::object::RemovedFieldsSignalType>(
            data::object::REMOVED_FIELDS_SIG
        );

        sig->async_emit(m_removedFields);
    }

    if(!m_newChangedFields.empty() && !m_oldChangedFields.empty())
    {
        auto sig = m_object.lock()->signal<data::object::ChangedFieldsSignalType>(
            data::object::CHANGED_FIELDS_SIG
        );

        sig->async_emit(m_newChangedFields, m_oldChangedFields);
    }

    if(!m_addedFields.empty())
    {
        auto sig = m_object.lock()->signal<data::object::AddedFieldsSignalType>(
            data::object::ADDED_FIELDS_SIG
        );

        sig->async_emit(m_addedFields);
    }

    SIGHT_INFO_IF(
        "No changes were found on the fields of the object '" + m_object.lock()->get_id()
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
    const data::object::FieldMapType& oldFields,
    const data::object::FieldMapType& newFields
)
{
    data::object::FieldNameVectorType oldFieldNames;
    data::object::FieldNameVectorType newFieldNames;

    std::transform(
        oldFields.begin(),
        oldFields.end(),
        std::back_inserter(oldFieldNames),
        [](const auto& e){return e.first;});
    std::transform(
        newFields.begin(),
        newFields.end(),
        std::back_inserter(newFieldNames),
        [](const auto& e){return e.first;});

    std::sort(oldFieldNames.begin(), oldFieldNames.end());
    std::sort(newFieldNames.begin(), newFieldNames.end());

    data::object::FieldNameVectorType added;   // new - old
    data::object::FieldNameVectorType changed; // old & new
    data::object::FieldNameVectorType removed; // old - new

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

    for(const data::object::FieldNameVectorType::value_type& fieldName : added)
    {
        m_addedFields[fieldName] = newFields.find(fieldName)->second;
    }

    for(const data::object::FieldNameVectorType::value_type& fieldName : changed)
    {
        m_newChangedFields[fieldName] = newFields.find(fieldName)->second;
        m_oldChangedFields[fieldName] = oldFields.find(fieldName)->second;
    }

    for(const data::object::FieldNameVectorType::value_type& fieldName : removed)
    {
        m_removedFields[fieldName] = oldFields.find(fieldName)->second;
    }
}

} // namespace sight::data::helper
