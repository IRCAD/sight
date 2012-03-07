/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_OBJECT_HPP_
#define _FWDATA_OBJECT_HPP_

#include <boost/unordered_map.hpp>
#include <string>

#include <boost/enable_shared_from_this.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/Field.hpp>
#include <fwTools/DynamicAttributes.hxx>
#include <fwTools/Factory.hpp>

#include "fwData/macros.hpp"
#include "fwData/config.hpp"

namespace fwData
{


/**
 * @brief   Base class for each data object.
 * Each object can optionally embed sub-object with string identifier. We name this concept a Field.
 * An Object containing a field name "dummy" corresponds to having a labeledObject with label "dummy" and
 * containing a specific Object. When accessing to this object with getField("dummy") we get the specific Object
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Object  : public ::fwTools::Object, public ::fwTools::DynamicAttributes< ::fwData::Object >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Object)(::fwTools::Object), (( )), ::fwTools::Factory::New< Object > );

    typedef std::string FieldNameType;
    typedef std::vector<FieldNameType> FieldNameVectorType;
    typedef ::boost::unordered_map< FieldNameType, ::fwData::Object::sptr > FieldMapType;

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] name Field name
     * @return null sptr if field is not found
     */
    FWDATA_API ::fwData::Object::sptr getField_NEWAPI( const FieldNameType & name ) const;

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] name Field name
     * @return null sptr if field is not found
     */
    FWDATA_API ::fwData::Object::csptr getConstField_NEWAPI( const FieldNameType & name ) const;

    /**
     * @brief Returns fields map.
     */
    FWDATA_API const FieldMapType& getFields_NEWAPI() const;

    /**
     * @brief Returns vector of field names.
     */
    FWDATA_API FieldNameVectorType getFieldNames_NEWAPI() const;

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     * @param[in] name Field name
     * @param[in] obj  Field
     */
    FWDATA_API void setField_NEWAPI( const FieldNameType & name, ::fwData::Object::sptr obj );

    /**
     * @brief Replace the field map content.
     */
    FWDATA_API void setFields_NEWAPI( const FieldMapType & fieldMap );

    /**
     * @brief Updates the field map content with fieldMap. Duplicated name will be replaced.
     */
    FWDATA_API void Object::updateFields_NEWAPI( const FieldMapType & fieldMap );

protected:

    /// Constructor
    FWDATA_API Object();

    /// Destructor
    FWDATA_API virtual ~Object() ;

    /// Fields
    FieldMapType m_fields;
};

} // fwData

#endif //_FWDATA_OBJECT_HPP_
