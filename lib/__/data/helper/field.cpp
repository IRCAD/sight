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

#include "data/helper/field.hpp"

#include <core/com/signal.hxx>

#include <data/exception.hpp>
#include <data/object.hpp>

#include <algorithm>
#include <functional>

namespace sight::data::helper
{

//-----------------------------------------------------------------------------

field::field(data::object::sptr _object) :
    m_object(_object)
{
}

//-----------------------------------------------------------------------------

field::~field()
{
    if(!m_addedfields.empty() || !m_new_changedfields.empty() || !m_removedfields.empty())
    {
        notify();
    }
}

//-----------------------------------------------------------------------------

void field::set_field(const data::object::field_name_t& _name, data::object::sptr _obj)
{
    this->add_or_swap(_name, _obj);
}

//-----------------------------------------------------------------------------

void field::set_fields(const data::object::field_map_t& _new_fields)
{
    data::object::sptr object = m_object.lock();
    SIGHT_ASSERT("field helper need a non-null object pointer", object);
    const data::object::field_map_t old_fields = object->get_fields();
    this->build_message(old_fields, _new_fields);
    object->set_fields(_new_fields);
}

//-----------------------------------------------------------------------------

void field::remove_field(const data::object::field_name_t& _name)
{
    this->remove(_name);
}

//-----------------------------------------------------------------------------

void field::add(const data::object::field_name_t& _name, data::object::sptr _obj)
{
    data::object::sptr object = m_object.lock();
    SIGHT_ASSERT("field helper need a non-null object pointer", object);

    data::object::sptr field = object->get_field(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("field already exists"), field);

    m_addedfields[_name] = _obj;

    object->set_field(_name, _obj);
}

//-----------------------------------------------------------------------------

// NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap,performance-noexcept-swap)
void field::swap(const data::object::field_name_t& _name, data::object::sptr _obj)
{
    SIGHT_ASSERT("field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();

    data::object::sptr field = object->get_field(_name);
    SIGHT_THROW_EXCEPTION_IF(data::exception("field does not exist"), !field);

    m_new_changedfields[_name] = _obj;
    m_old_changedfields[_name] = field;

    object->set_field(_name, _obj);
}

//-----------------------------------------------------------------------------

void field::add_or_swap(const data::object::field_name_t& _name, data::object::sptr _obj)
{
    SIGHT_ASSERT("field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();

    data::object::sptr field = object->get_field(_name);

    if(!field)
    {
        m_addedfields[_name] = _obj;
    }
    else
    {
        m_new_changedfields[_name] = _obj;
        m_old_changedfields[_name] = field;
    }

    object->set_field(_name, _obj);
}

//-----------------------------------------------------------------------------

void field::remove(const data::object::field_name_t& _name)
{
    SIGHT_ASSERT("field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();
    data::object::sptr field  = object->get_field(_name);

    SIGHT_THROW_EXCEPTION_IF(data::exception("field does not exist"), !field);

    m_removedfields[_name] = field;
    object->remove_field(_name);
}

//-----------------------------------------------------------------------------

void field::clear()
{
    SIGHT_ASSERT("field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();

    auto field_names = object->get_field_names();
    for(const auto& name : field_names)
    {
        data::object::sptr field = object->get_field(name);
        m_removedfields[name] = field;
        object->remove_field(name);
    }
}

//-----------------------------------------------------------------------------

void field::notify()
{
    SIGHT_ASSERT("field helper need a non-null object pointer", !m_object.expired());

    if(!m_removedfields.empty())
    {
        auto sig = m_object.lock()->signal<data::object::removed_fields_signal_t>(
            data::object::REMOVED_FIELDS_SIG
        );

        sig->async_emit(m_removedfields);
    }

    if(!m_new_changedfields.empty() && !m_old_changedfields.empty())
    {
        auto sig = m_object.lock()->signal<data::object::changed_fields_signal_t>(
            data::object::CHANGED_FIELDS_SIG
        );

        sig->async_emit(m_new_changedfields, m_old_changedfields);
    }

    if(!m_addedfields.empty())
    {
        auto sig = m_object.lock()->signal<data::object::added_fields_signal_t>(
            data::object::ADDED_FIELDS_SIG
        );

        sig->async_emit(m_addedfields);
    }

    SIGHT_INFO_IF(
        "No changes were found on the fields of the object '" + m_object.lock()->get_id()
        + "', nothing to notify.",
        m_addedfields.empty() && m_new_changedfields.empty() && m_removedfields.empty()
    );

    m_removedfields.clear();
    m_new_changedfields.clear();
    m_old_changedfields.clear();
    m_addedfields.clear();
}

//-----------------------------------------------------------------------------

void field::build_message(
    const data::object::field_map_t& _old_fields,
    const data::object::field_map_t& _new_fields
)
{
    data::object::field_name_vector_t old_field_names;
    data::object::field_name_vector_t new_field_names;

    std::transform(
        _old_fields.begin(),
        _old_fields.end(),
        std::back_inserter(old_field_names),
        [](const auto& _e){return _e.first;});
    std::transform(
        _new_fields.begin(),
        _new_fields.end(),
        std::back_inserter(new_field_names),
        [](const auto& _e){return _e.first;});

    std::sort(old_field_names.begin(), old_field_names.end());
    std::sort(new_field_names.begin(), new_field_names.end());

    data::object::field_name_vector_t added;   // new - old
    data::object::field_name_vector_t changed; // old & new
    data::object::field_name_vector_t removed; // old - new

    std::set_difference(
        new_field_names.begin(),
        new_field_names.end(),
        old_field_names.begin(),
        old_field_names.end(),
        std::back_inserter(added)
    );

    std::set_intersection(
        new_field_names.begin(),
        new_field_names.end(),
        old_field_names.begin(),
        old_field_names.end(),
        std::back_inserter(changed)
    );

    std::set_difference(
        old_field_names.begin(),
        old_field_names.end(),
        new_field_names.begin(),
        new_field_names.end(),
        std::back_inserter(removed)
    );

    for(const auto& field_name : added)
    {
        m_addedfields[field_name] = _new_fields.find(field_name)->second;
    }

    for(const auto& field_name : changed)
    {
        m_new_changedfields[field_name] = _new_fields.find(field_name)->second;
        m_old_changedfields[field_name] = _old_fields.find(field_name)->second;
    }

    for(const auto& field_name : removed)
    {
        m_removedfields[field_name] = _old_fields.find(field_name)->second;
    }
}

} // namespace sight::data::helper
