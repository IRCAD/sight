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
    fwCoreAllowSharedFromThis();

    typedef std::string FieldNameType;
    typedef std::vector<FieldNameType> FieldNameVectorType;
    typedef ::boost::unordered_map< FieldNameType, ::fwData::Object::sptr > FieldMapType;

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] name Field name
     * @param[in] defaultValue Default value
     * @return defaultValue if field is not found
     */
    FWDATA_API ::fwData::Object::sptr getField_NEWAPI( const FieldNameType & name, ::fwData::Object::sptr defaultValue = ::fwData::Object::sptr() ) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] name Field name
     * @return pointer to corresponding field, nullptr if field is not found.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) getField_NEWAPI( const FieldNameType& name ) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] name Field name
     * @param[in] defaultValue Default value
     * @return pointer to corresponding field, defaultValue if field is not found.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) getField_NEWAPI( const FieldNameType& name, SPTR(DATA_TYPE) defaultValue ) const;

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if defaultValue is not null.
     * @param[in] name Field name
     * @param[in] defaultValue default return value if field was not found
     * @return pointer to corresponding field.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) setDefaultField_NEWAPI( const FieldNameType& name, SPTR(DATA_TYPE) defaultValue );

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
     * @brief Removes field with specified name.
     * @param[in] name Field name
     */
    FWDATA_API void removeField( const FieldNameType & name ) ;

    /**
     * @brief Updates the field map content with fieldMap. Duplicated name will be replaced.
     */
    FWDATA_API void updateFields_NEWAPI( const FieldMapType & fieldMap );

    /**
     * @brief A shallow copy of fields (objects in m_children)
     * @param[in] _source source of the copy.
     */
    FWDATA_API virtual void shallowCopy( ::fwData::Object::csptr source );

    /**
     * @brief A deep copy of fields (objects in m_children)
     * @param[in] _source source of the copy.
     */
    FWDATA_API virtual void deepCopy( ::fwData::Object::csptr source );

    /**
     * @brief return a copy of the source. if source is a null pointer, return a null pointer.
     * @{
     */
    FWDATA_API static ::fwData::Object::sptr copy(::fwData::Object::csptr source);
    template <typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(SPTR(DATA_TYPE) source);
    /** @} */

    /**
     * @brief A shallow copy of fields (objects in m_children)
     * @param[in] _source source of the copy.
     */
    FWDATA_API void fieldShallowCopy( ::fwData::Object::csptr source );

    /**
     * @brief A deep copy of fields (objects in m_children)
     * @param[in] _source source of the copy.
     */
    FWDATA_API void fieldDeepCopy( ::fwData::Object::csptr source );

    //-----------------------------------------------------------------------------

    template< typename DATA_TYPE >
    void shallowCopy( ::fwData::Object::csptr source )
    {
        typename DATA_TYPE::csptr castSource = DATA_TYPE::dynamicConstCast( source );
        SLM_FATAL_IF("Sorry, the classname of object source is different, shallowCopy is not possible.", castSource == 0 );
        typename DATA_TYPE::sptr castDest = DATA_TYPE::dynamicCast( this->getSptr() );
        castDest->DATA_TYPE::shallowCopy( castSource );
    }

    //-----------------------------------------------------------------------------

    template< typename DATA_TYPE >
    void deepCopy( ::fwData::Object::csptr source )
    {
        typename DATA_TYPE::csptr castSource = DATA_TYPE::dynamicConstCast( source );
        SLM_FATAL_IF("Sorry, the classname of object source is different, deepCopy is not possible.", castSource == 0 );
        typename DATA_TYPE::sptr castDest = DATA_TYPE::dynamicCast( this->getSptr() );
        castDest->DATA_TYPE::deepCopy( castSource );
    }

protected:

    /// Constructor
    FWDATA_API Object();

    /// Destructor
    FWDATA_API virtual ~Object() ;

    /// Fields
    FieldMapType m_fields;
};


template <typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(SPTR(DATA_TYPE) source)
{
    return DATA_TYPE::dynamicCast( ::fwData::Object::copy( ::fwData::Object::csptr(source)) );
}

//-----------------------------------------------------------------------------

template< typename DATA_TYPE >
SPTR(DATA_TYPE) Object::getField_NEWAPI( const FieldNameType& name ) const
{
    ::fwData::Object::sptr field;
    field = this->getField_NEWAPI( name, field );
    SPTR(DATA_TYPE) result  = DATA_TYPE::dynamicCast( field );
    return result;
}

//-----------------------------------------------------------------------------

template< typename DATA_TYPE >
SPTR(DATA_TYPE) Object::getField_NEWAPI( const FieldNameType& name, SPTR(DATA_TYPE) defaultValue ) const
{
    ::fwData::Object::sptr field = defaultValue;
    field = this->getField_NEWAPI( name, field );
    SPTR(DATA_TYPE) result  = DATA_TYPE::dynamicCast( field );
    return result;
}

//-----------------------------------------------------------------------------

template< typename DATA_TYPE >
SPTR(DATA_TYPE) Object::setDefaultField_NEWAPI( const FieldNameType& name, SPTR(DATA_TYPE) defaultValue )
{
    SPTR(DATA_TYPE) result = getField_NEWAPI< DATA_TYPE >(name);
    if( !result && defaultValue)
    {
        result = defaultValue;
        this->setField_NEWAPI(name, defaultValue);
    }
    return result;
}

} // namespace fwData

#endif //_FWDATA_OBJECT_HPP_
