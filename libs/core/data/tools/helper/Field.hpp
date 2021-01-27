/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Object.hpp>

namespace sight::data::tools
{
namespace helper
{

/**
 * @brief   Defines a helper to modify field on a data::Object and create a message notifying this modification.
 */
class DATA_CLASS_API Field
{

public:

    /// Constructor. Initialize parameters.
    DATA_API Field( data::Object::sptr object );

    /// Destructor. Call notify if changes has been made.
    DATA_API ~Field();

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if
     * defaultValue is not null.
     * @param[in] name Field name
     * @param[in] defaultValue default return value if field was not found
     * @return pointer to corresponding field.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) setDefaultField( const data::Object::FieldNameType& name, SPTR(DATA_TYPE) defaultValue );

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     * * @deprecated use addOrSwap() instead
     */
    DATA_API void setField( const data::Object::FieldNameType& name, data::Object::sptr obj );

    /**
     * @brief Replace the field map content.
     */
    DATA_API void setFields( const data::Object::FieldMapType& newFields );

    /**
     * @brief Removes field with specified name.
     * @deprecated use remove() instead
     */
    DATA_API void removeField( const data::Object::FieldNameType& name );

    /**
     * @brief Add a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add as a field
     *
     * @throw data::Exception if the field already exists
     * Prepare the message to announce the modification.
     */
    DATA_API void add( const data::Object::FieldNameType& _name, data::Object::sptr _obj );

    /**
     * @brief Replace a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add  as a field
     *
     * @throw data::Exception if the field does not exist.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void swap( const data::Object::FieldNameType& _name, data::Object::sptr _obj );

    /**
     * @brief Add or replace a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add  as a field
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void addOrSwap( const data::Object::FieldNameType& _name, data::Object::sptr _obj );

    /**
     * @brief Remove a field from the object.
     * @param[in] _name  key of the field.
     *
     * @throw data::Exception if the field does not exist.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void remove(  const data::Object::FieldNameType& _name );

    /**
     * @brief Clear all fields in the object.
     *
     * Prepare the message to announce the modification.
     */
    DATA_API void clear();

    /// Send the built message and clear the internal maps
    DATA_API void notify();

protected:
    DATA_API void buildMessage(
        const data::Object::FieldMapType& oldFields,
        const data::Object::FieldMapType& newFields
        );

    /// Map of added objects, send on notify
    data::Object::FieldsContainerType m_addedFields;
    /// Map of new changed objects, send on notify
    data::Object::FieldsContainerType m_newChangedFields;
    /// Map of old changed objects, send on notify
    data::Object::FieldsContainerType m_oldChangedFields;
    /// Map of removed objects, send on notify
    data::Object::FieldsContainerType m_removedFields;
    /// Composite to add/remove/change objects

    data::Object::wptr m_object;
};

template<typename DATA_TYPE>
inline SPTR(DATA_TYPE) Field::setDefaultField(const data::Object::FieldNameType& name, SPTR(DATA_TYPE) defaultValue)
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    data::Object::sptr object = m_object.lock();
    data::Object::sptr field  = object->getField(name);
    if (!field)
    {
        m_addedFields[name] = defaultValue;
    }
    return object->setDefaultField(name, defaultValue);
}

} // namespace helper
} // namespace sight::data::tools
