/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#pragma once

#include "data/config.hpp"

#include <data/object.hpp>

namespace sight::data::helper
{

/**
 * @brief   Defines a helper to modify field on a data::object and create a message notifying this modification.
 */
class DATA_CLASS_API field
{
public:

    /// Constructor. Initialize parameters.
    DATA_API field(data::object::sptr _object);

    /// Destructor. Call notify if changes has been made.
    DATA_API ~field();

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if
     * defaultValue is not null.
     * @param[in] _name field name
     * @param[in] _default_value default return value if field was not found
     * @return pointer to corresponding field.
     */
    template<typename DATA_TYPE>
    SPTR(DATA_TYPE) set_default_field(const data::object::field_name_t& _name, SPTR(DATA_TYPE) _default_value);

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     * * @deprecated use addOrSwap() instead
     */
    DATA_API void set_field(const data::object::field_name_t& _name, data::object::sptr _obj);

    /**
     * @brief Replace the field map content.
     */
    DATA_API void set_fields(const data::object::field_map_t& _new_fields);

    /**
     * @brief Removes field with specified name.
     * @deprecated use remove() instead
     */
    DATA_API void remove_field(const data::object::field_name_t& _name);

    /**
     * @brief Add a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add as a field
     *
     * @throw data::exception if the field already exists
     * Prepare the message to announce the modification.
     */
    DATA_API void add(const data::object::field_name_t& _name, data::object::sptr _obj);

    /**
     * @brief Replace a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add  as a field
     *
     * @throw data::exception if the field does not exist.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void swap(const data::object::field_name_t& _name, data::object::sptr _obj);

    /**
     * @brief Add or replace a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add  as a field
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void add_or_swap(const data::object::field_name_t& _name, data::object::sptr _obj);

    /**
     * @brief Remove a field from the object.
     * @param[in] _name  key of the field.
     *
     * @throw data::exception if the field does not exist.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void remove(const data::object::field_name_t& _name);

    /**
     * @brief Clear all fields in the object.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void clear();

    /// Send the built message and clear the internal maps
    DATA_API void notify();

protected:

    DATA_API void build_message(
        const data::object::field_map_t& _old_fields,
        const data::object::field_map_t& _new_fields
    );

    /// Map of added objects, send on notify
    data::object::fields_container_t m_addedfields;
    /// Map of new changed objects, send on notify
    data::object::fields_container_t m_new_changedfields;
    /// Map of old changed objects, send on notify
    data::object::fields_container_t m_old_changedfields;
    /// Map of removed objects, send on notify
    data::object::fields_container_t m_removedfields;
    /// Composite to add/remove/change objects

    data::object::wptr m_object;
};

template<typename DATA_TYPE>
inline SPTR(DATA_TYPE) field::set_default_field(const data::object::field_name_t& _name, SPTR(DATA_TYPE) _default_value)
{
    SIGHT_ASSERT("field helper need a non-null object pointer", !m_object.expired());
    data::object::sptr object = m_object.lock();
    data::object::sptr field  = object->get_field(_name);
    if(!field)
    {
        m_addedfields[_name] = _default_value;
    }

    return object->set_default_field(_name, _default_value);
}

} // namespace sight::data::helper
