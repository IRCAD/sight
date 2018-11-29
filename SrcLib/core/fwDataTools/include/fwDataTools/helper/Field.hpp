/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWDATATOOLS_HELPER_FIELD_HPP__
#define __FWDATATOOLS_HELPER_FIELD_HPP__

#include "fwDataTools/config.hpp"

#include <fwData/Object.hpp>

namespace fwDataTools
{
namespace helper
{

/**
 * @brief   Defines a helper to modify field on a ::fwData::Object and create a message notifying this modification.
 */
class FWDATATOOLS_CLASS_API Field
{

public:

    /// Constructor. Initialize parameters.
    FWDATATOOLS_API Field( ::fwData::Object::sptr object );

    /// Destructor. Call notify if changes has been made.
    FWDATATOOLS_API ~Field();

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if
     * defaultValue is not null.
     * @param[in] name Field name
     * @param[in] defaultValue default return value if field was not found
     * @return pointer to corresponding field.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) setDefaultField( const ::fwData::Object::FieldNameType& name, SPTR(DATA_TYPE) defaultValue );

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     * * @deprecated use addOrSwap() instead
     */
    FWDATATOOLS_API void setField( const ::fwData::Object::FieldNameType& name, ::fwData::Object::sptr obj );

    /**
     * @brief Replace the field map content.
     */
    FWDATATOOLS_API void setFields( const ::fwData::Object::FieldMapType& newFields );

    /**
     * @brief Removes field with specified name.
     * @deprecated use remove() instead
     */
    FWDATATOOLS_API void removeField( const ::fwData::Object::FieldNameType& name );

    /**
     * @brief Add a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add as a field
     *
     * @throw ::fwData::Exception if the field already exists
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void add( const ::fwData::Object::FieldNameType& _name, ::fwData::Object::sptr _obj );

    /**
     * @brief Replace a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add  as a field
     *
     * @throw ::fwData::Exception if the field does not exist.
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void swap( const ::fwData::Object::FieldNameType& _name, ::fwData::Object::sptr _obj );

    /**
     * @brief Add or replace a field in the object.
     * @param[in] _name key of the field.
     * @param[in] _obj object to add  as a field
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void addOrSwap( const ::fwData::Object::FieldNameType& _name, ::fwData::Object::sptr _obj );

    /**
     * @brief Remove a field from the object.
     * @param[in] _name  key of the field.
     *
     * @throw ::fwData::Exception if the field does not exist.
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void remove(  const ::fwData::Object::FieldNameType& _name );

    /**
     * @brief Clear all fields in the object.
     *
     * Prepare the message to announce the modification.
     */
    FWDATATOOLS_API void clear();

    /// Send the built message and clear the internal maps
    FWDATATOOLS_API void notify();

protected:
    FWDATATOOLS_API void buildMessage(
        const ::fwData::Object::FieldMapType& oldFields,
        const ::fwData::Object::FieldMapType& newFields
        );

    /// Map of added objects, send on notify
    ::fwData::Object::FieldsContainerType m_addedFields;
    /// Map of new changed objects, send on notify
    ::fwData::Object::FieldsContainerType m_newChangedFields;
    /// Map of old changed objects, send on notify
    ::fwData::Object::FieldsContainerType m_oldChangedFields;
    /// Map of removed objects, send on notify
    ::fwData::Object::FieldsContainerType m_removedFields;
    /// Composite to add/remove/change objects

    ::fwData::Object::wptr m_object;
};

template<typename DATA_TYPE>
inline SPTR(DATA_TYPE) Field::setDefaultField(const fwData::Object::FieldNameType& name, SPTR(DATA_TYPE) defaultValue)
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    ::fwData::Object::sptr object = m_object.lock();
    ::fwData::Object::sptr field  = object->getField(name);
    if (!field)
    {
        m_addedFields[name] = defaultValue;
    }
    return object->setDefaultField(name, defaultValue);
}

} // namespace helper
} // namespace fwDataTools

#endif // __FWDATATOOLS_HELPER_FIELD_HPP__
