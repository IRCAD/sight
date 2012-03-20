/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_HELPER_FIELD_HPP_
#define _FWCOMED_HELPER_FIELD_HPP_

#include <fwServices/IService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwData/Object.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{
namespace helper
{

/**
 * @brief   Defines an helper to modify field on a ::fwData::Object and create a message notifying this modification.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWCOMED_CLASS_API Field
{

public :

    /// Constructor. Initialize parameters.
    FWCOMED_API Field( ::fwData::Object::sptr object );

    /// Destructor. Do nothing.
    FWCOMED_API ~Field();

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
    FWCOMED_API void setField( const ::fwData::Object::FieldNameType & name, ::fwData::Object::sptr obj );

    /**
     * @brief Replace the field map content.
     */
    FWCOMED_API void setFields( const ::fwData::Object::FieldMapType & newFields );

    /**
     * @brief Updates the field map content with fieldMap. Duplicated name will be replaced.
     */
    FWCOMED_API void updateFields( const ::fwData::Object::FieldMapType & fieldMap );

    /**
     * @brief Removes field with specified name.
     */
    FWCOMED_API void removeField( const ::fwData::Object::FieldNameType &name );

    /// Send the built message
    FWCOMED_API void notify( ::fwServices::IService::sptr _serviceSource );

protected:
    FWCOMED_API void buildMessage(
            const ::fwData::Object::FieldMapType &oldFields,
            const ::fwData::Object::FieldMapType &newFields
            );

    ::fwServices::ObjectMsg::sptr m_objectMsg;

    ::fwData::Object::wptr m_object;
};



template<typename DATA_TYPE>
inline SPTR(DATA_TYPE) Field::setDefaultField(const fwData::Object::FieldNameType& name, SPTR(DATA_TYPE) defaultValue)
{
    SLM_ASSERT("Field helper need a non-null object pointer", !m_object.expired());
    ::fwData::Object::sptr object = m_object.lock();
    ::fwData::Object::sptr field = object->getField_NEWAPI(name);
    if (!field)
    {
        m_objectMsg->appendAddedField(name, defaultValue);
    }
    return object->setDefaultField_NEWAPI(name, defaultValue);
}

} // namespace helper
} // namespace fwComEd

#endif // _FWCOMED_HELPER_FIELD_HPP_
