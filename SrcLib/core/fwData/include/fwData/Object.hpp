/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_OBJECT_HPP__
#define __FWDATA_OBJECT_HPP__

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/registry/detail.hpp"

#include <fwCamp/macros.hpp>
#ifdef CAMP_COMPILATION
#include <fwCamp/Mapper/ArrayMapper.hpp>
#include <fwCamp/camp/ExtendedClassVisitor.hpp>
#include <fwCamp/camp/MapMapper.hpp>
#include <fwCamp/camp/MapProperty.hpp>
#include <fwCamp/camp/MapValueMapper.hpp>
#include <fwCamp/camp/customtype.hpp>
#include <fwCamp/camp/detail/MapPropertyImpl.hpp>
#endif

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>

#include <fwCore/base.hpp>
#include <fwCore/mt/types.hpp>

#include <fwTools/Object.hpp>

#include <string>
#include <unordered_map>

fwCampAutoDeclareDataMacro((fwData)(Object), FWDATA_API);

namespace fwData
{

/**
 * @brief   Base class for each data object.
 *
 * Each object can optionally embed sub-object with string identifier. We name this concept a Field.
 * An Object containing a field name "dummy" corresponds to having a labeledObject with label "dummy" and
 * containing a specific Object. When accessing to this object with getField("dummy") we get the specific Object
 */
class FWDATA_CLASS_API Object  : public ::fwTools::Object,
                                 public ::fwCom::HasSignals
{
public:

    typedef ::fwData::factory::Key Key;

    fwCoreNonInstanciableClassDefinitionsMacro( (Object)(::fwTools::Object) );
    fwCoreAllowSharedFromThis();
    fwCampMakeFriendDataMacro((fwData)(Object));

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar()
        {
            ::fwData::registry::get()->addFactory(T::classname(), &::fwData::factory::New<T>);
        }
    };

    /**
     * @name Signals
     * @{
     */
    /// Type of signal m_sigModified
    typedef ::fwCom::Signal< void () > ModifiedSignalType;

    /// Key in m_signals map of signal m_sigModified
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_MODIFIED_SIG;

    typedef std::map<std::string, ::fwData::Object::sptr> FieldsContainerType;
    /// Type of signal when objects are added
    typedef ::fwCom::Signal< void (FieldsContainerType) > AddedFieldsSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_ADDED_FIELDS_SIG;

    /// Type of signal when objects are changed (newObjects, oldObjects)
    typedef ::fwCom::Signal< void (FieldsContainerType, FieldsContainerType) > ChangedFieldsSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_CHANGED_FIELDS_SIG;

    /// Type of signal when objects are removed
    typedef ::fwCom::Signal< void (FieldsContainerType) > RemovedFieldsSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_REMOVED_FIELDS_SIG;
    /**
     * @}
     */

    typedef std::string FieldNameType;
    typedef std::vector<FieldNameType> FieldNameVectorType;
    typedef ::std::unordered_map< FieldNameType, ::fwData::Object::sptr > FieldMapType;

    typedef ::std::unordered_map< ::fwData::Object::csptr, ::fwData::Object::sptr > DeepCopyCacheType;

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] name Field name
     * @param[in] defaultValue Default value
     * @return defaultValue if field is not found
     */
    FWDATA_API ::fwData::Object::sptr getField( const FieldNameType& name,
                                                ::fwData::Object::sptr defaultValue = ::fwData::Object::sptr() ) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] name Field name
     * @return pointer to corresponding field, nullptr if field is not found.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) getField( const FieldNameType &name ) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] name Field name
     * @param[in] defaultValue Default value
     * @return pointer to corresponding field, defaultValue if field is not found.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) getField( const FieldNameType &name, SPTR(DATA_TYPE) defaultValue ) const;

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if
     * defaultValue is not null.
     * @param[in] name Field name
     * @param[in] defaultValue default return value if field was not found
     * @return pointer to corresponding field.
     */
    template< typename DATA_TYPE >
    SPTR(DATA_TYPE) setDefaultField( const FieldNameType &name, SPTR(DATA_TYPE) defaultValue );

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] name Field name
     * @return null sptr if field is not found
     */
    FWDATA_API ::fwData::Object::csptr getConstField( const FieldNameType& name ) const;

    /**
     * @brief Returns fields map.
     */
    FWDATA_API const FieldMapType& getFields() const;

    /**
     * @brief Returns vector of field names.
     */
    FWDATA_API FieldNameVectorType getFieldNames() const;

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     * @param[in] name Field name
     * @param[in] obj  Field
     */
    FWDATA_API void setField( const FieldNameType& name, ::fwData::Object::sptr obj );

    /**
     * @brief Replace the field map content.
     */
    FWDATA_API void setFields( const FieldMapType& fieldMap );

    /**
     * @brief Removes field with specified name.
     * @param[in] name Field name
     */
    FWDATA_API void removeField( const FieldNameType& name );

    /**
     * @brief A shallow copy of fields (objects in m_children)
     * @param[in] source source of the copy.
     */
    FWDATA_API virtual void shallowCopy( const ::fwData::Object::csptr& source );

    /**
     * @brief Make a deep copy from the source
     * Calling this method may invalidate any DumpLock, RescursiveLock or helper
     * on the object. Prefer using fwData::Object::copy instead.
     */
    FWDATA_API void deepCopy( const ::fwData::Object::csptr& source );

    /**
     * @brief return a copy of the source. if source is a null pointer, return a null pointer.
     * @{
     */
    FWDATA_API static ::fwData::Object::sptr copy(const ::fwData::Object::csptr& source);
    template <typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const CSPTR(DATA_TYPE) &source);
    template <typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const SPTR(DATA_TYPE) &source);
    /** @} */

    /**
     * @brief A shallow copy of fields (objects in m_children)
     * @param[in] source source of the copy.
     */
    FWDATA_API void fieldShallowCopy( const ::fwData::Object::csptr& source );

    /**
     * @brief A deep copy of fields (objects in m_children)
     * @param[in] source source of the copy.
     */
    FWDATA_API void fieldDeepCopy( const ::fwData::Object::csptr& source );

    //-----------------------------------------------------------------------------

    /// Returns the object's mutex.
    ::fwCore::mt::ReadWriteMutex &getMutex() const
    {
        return m_mutex;
    }

    FWDATA_API virtual ~Object();

protected:

    FWDATA_API Object();

    /**
     * @brief Internal-use methods to implement Object's deepCopy
     * @{
     */
    FWDATA_API static ::fwData::Object::sptr copy(const ::fwData::Object::csptr& source, DeepCopyCacheType& cache);
    FWDATA_API void fieldDeepCopy( const ::fwData::Object::csptr& source, DeepCopyCacheType& cache );
    FWDATA_API virtual void cachedDeepCopy(const ::fwData::Object::csptr& source, DeepCopyCacheType& cache) = 0;
    template <typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const CSPTR(DATA_TYPE) &source, DeepCopyCacheType &cache);
    template <typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const SPTR(DATA_TYPE) &source, DeepCopyCacheType &cache);
    /** @} */

    /// Fields
    FieldMapType m_fields;

    /// Mutex to protect object access.
    mutable ::fwCore::mt::ReadWriteMutex m_mutex;
};

template <typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const CSPTR(DATA_TYPE) &source, DeepCopyCacheType &cache)
{
    return DATA_TYPE::dynamicCast( ::fwData::Object::copy(::fwData::Object::csptr(source), cache) );
}

template <typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const SPTR(DATA_TYPE) &source, DeepCopyCacheType &cache)
{
    return DATA_TYPE::dynamicCast( ::fwData::Object::copy(::fwData::Object::csptr(source), cache) );
}

template <typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const CSPTR(DATA_TYPE) &source)
{
    return DATA_TYPE::dynamicCast( ::fwData::Object::copy(::fwData::Object::csptr(source)) );
}

template <typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const SPTR(DATA_TYPE) &source)
{
    return DATA_TYPE::dynamicCast( ::fwData::Object::copy(::fwData::Object::csptr(source)) );
}

//-----------------------------------------------------------------------------

template< typename DATA_TYPE >
SPTR(DATA_TYPE) Object::getField( const FieldNameType &name ) const
{
    ::fwData::Object::sptr field;
    field                  = this->getField( name, field );
    SPTR(DATA_TYPE) result = DATA_TYPE::dynamicCast( field );
    return result;
}

//-----------------------------------------------------------------------------

template< typename DATA_TYPE >
SPTR(DATA_TYPE) Object::getField( const FieldNameType &name, SPTR(DATA_TYPE) defaultValue ) const
{
    ::fwData::Object::sptr field = defaultValue;
    field                        = this->getField( name, field );
    SPTR(DATA_TYPE) result       = DATA_TYPE::dynamicCast( field );
    return result;
}

//-----------------------------------------------------------------------------

template< typename DATA_TYPE >
SPTR(DATA_TYPE) Object::setDefaultField( const FieldNameType &name, SPTR(DATA_TYPE) defaultValue )
{
    SPTR(DATA_TYPE) result = getField< DATA_TYPE >(name);
    if( !result && defaultValue)
    {
        result = defaultValue;
        this->setField(name, defaultValue);
    }
    return result;
}

} // namespace fwData

#endif //__FWDATA_OBJECT_HPP__
