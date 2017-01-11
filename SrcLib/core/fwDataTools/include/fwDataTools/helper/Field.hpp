/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    /// Destructor. Do nothing.
    FWDATATOOLS_API ~Field();

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if defaultValue is not null.
     * @param[in] name Field name
     * @param[in] defaultValue default return value if field was not found
     * @return pointer to corresponding field.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) setDefaultField( const ::fwData::Object::FieldNameType& name, SPTR(DATA_TYPE) defaultValue );

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     */
    FWDATATOOLS_API void setField( const ::fwData::Object::FieldNameType& name, ::fwData::Object::sptr obj );

    /**
     * @brief Replace the field map content.
     */
    FWDATATOOLS_API void setFields( const ::fwData::Object::FieldMapType& newFields );

    /**
     * @brief Updates the field map content with fieldMap. Duplicated name will be replaced.
     */
    FWDATATOOLS_API void updateFields( const ::fwData::Object::FieldMapType& fieldMap );

    /**
     * @brief Removes field with specified name.
     */
    FWDATATOOLS_API void removeField( const ::fwData::Object::FieldNameType& name );

    /// Send the built message
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
