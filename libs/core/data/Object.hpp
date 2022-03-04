/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/registry/detail.hpp"

#include <core/base.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/mt/types.hpp>
#include <core/tools/compare.hpp>
#include <core/tools/Object.hpp>

#include <string>
#include <unordered_map>

namespace sight::data
{

/**
 * @brief   Base class for each data object.
 *
 * Each object can optionally embed sub-object with string identifier. We name this concept a Field.
 * An Object containing a field name "dummy" corresponds to having a labeledObject with label "dummy" and
 * containing a specific Object. When accessing to this object with getField("dummy") we get the specific Object
 */
class DATA_CLASS_API Object : public core::tools::Object,
                              public core::com::HasSignals
{
public:

    typedef factory::Key Key;

    SIGHT_DECLARE_CLASS(Object, core::tools::Object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template<typename T>
    class Registry
    {
    public:

        Registry()
        {
            registry::get()->addFactory(T::classname(), &factory::New<T>);
        }
    };

    /**
     * @name Signals
     * @{
     */
    /// Type of signal m_sigModified
    typedef core::com::Signal<void ()> ModifiedSignalType;

    /// Key in m_signals map of signal m_sigModified
    DATA_API static const core::com::Signals::SignalKeyType s_MODIFIED_SIG;

    typedef std::map<std::string, Object::sptr> FieldsContainerType;
    /// Type of signal when objects are added
    typedef core::com::Signal<void (FieldsContainerType)> AddedFieldsSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_ADDED_FIELDS_SIG;

    /// Type of signal when objects are changed (newObjects, oldObjects)
    typedef core::com::Signal<void (FieldsContainerType, FieldsContainerType)> ChangedFieldsSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_CHANGED_FIELDS_SIG;

    /// Type of signal when objects are removed
    typedef core::com::Signal<void (FieldsContainerType)> RemovedFieldsSignalType;
    DATA_API static const core::com::Signals::SignalKeyType s_REMOVED_FIELDS_SIG;
    /**
     * @}
     */

    typedef std::string FieldNameType;
    typedef std::vector<FieldNameType> FieldNameVectorType;
    typedef std::unordered_map<FieldNameType, Object::sptr> FieldMapType;

    typedef std::unordered_map<Object::csptr, Object::sptr> DeepCopyCacheType;

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] name Field name
     * @param[in] defaultValue Default value
     * @return defaultValue if field is not found
     */
    DATA_API Object::sptr getField(
        const FieldNameType& name,
        Object::sptr defaultValue = Object::sptr()
    ) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] name Field name
     * @return pointer to corresponding field, nullptr if field is not found.
     */
    template<typename DATA_TYPE>
    SPTR(DATA_TYPE) getField(const FieldNameType& name) const;

    /**
     * @brief Returns a pointer of corresponding field.
     * @param[in] name Field name
     * @param[in] defaultValue Default value
     * @return pointer to corresponding field, defaultValue if field is not found.
     */
    template<typename DATA_TYPE>
    SPTR(DATA_TYPE) getField(const FieldNameType& name, SPTR(DATA_TYPE) defaultValue) const;

    /**
     * @brief Returns a pointer of corresponding field. If field did not exist, it is set to defaultValue if
     * defaultValue is not null.
     * @param[in] name Field name
     * @param[in] defaultValue default return value if field was not found
     * @return pointer to corresponding field.
     */
    template<typename DATA_TYPE>
    SPTR(DATA_TYPE) setDefaultField(const FieldNameType& name, SPTR(DATA_TYPE) defaultValue);

    /**
     * @brief Returns a pointer of corresponding field (null if non exist).
     * @param[in] name Field name
     * @return null sptr if field is not found
     */
    DATA_API Object::csptr getConstField(const FieldNameType& name) const;

    /**
     * @brief Returns fields map.
     */
    DATA_API const FieldMapType& getFields() const;

    /**
     * @brief Returns vector of field names.
     */
    DATA_API FieldNameVectorType getFieldNames() const;

    /**
     * @brief Register field with specified name. If the name does already exist, the matching field will be replaced.
     * @param[in] name Field name
     * @param[in] obj  Field
     */
    DATA_API void setField(const FieldNameType& name, Object::sptr obj);

    /**
     * @brief Replace the field map content.
     */
    DATA_API void setFields(const FieldMapType& fieldMap);

    /**
     * @brief Removes field with specified name.
     * @param[in] name Field name
     */
    DATA_API void removeField(const FieldNameType& name);

    /**
     * @brief A shallow copy of fields (objects in m_children)
     * @param[in] source source of the copy.
     */
    DATA_API virtual void shallowCopy(const Object::csptr& source);

    /**
     * @brief Make a deep copy from the source
     * Calling this method may invalidate any DumpLock, RescursiveLock or helper
     * on the object. Prefer using Object::copy instead.
     */
    DATA_API void deepCopy(const Object::csptr& source);

    /**
     * @brief return a copy of the source. if source is a null pointer, return a null pointer.
     * @{
     */
    DATA_API static Object::sptr copy(const Object::csptr& source);
    template<typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const CSPTR(DATA_TYPE) & source);
    template<typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const SPTR(DATA_TYPE) & source);
    /** @} */

    /**
     * @brief A shallow copy of fields (objects in m_children)
     * @param[in] source source of the copy.
     */
    DATA_API void fieldShallowCopy(const Object::csptr& source);

    /**
     * @brief A deep copy of fields (objects in m_children)
     * @param[in] source source of the copy.
     */
    DATA_API void fieldDeepCopy(const Object::csptr& source);

    //-----------------------------------------------------------------------------

    /// Returns the object's mutex.
    inline core::mt::ReadWriteMutex& getMutex() const
    {
        return m_mutex;
    }

    DATA_API virtual ~Object();

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Object& other) const noexcept;
    DATA_API bool operator!=(const Object& other) const noexcept;
    /// @}

protected:

    DATA_API Object();

    /**
     * @brief Internal-use methods to implement Object's deepCopy
     * @{
     */
    DATA_API static Object::sptr copy(const Object::csptr& source, DeepCopyCacheType& cache);
    DATA_API void fieldDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache);
    DATA_API virtual void cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache) = 0;
    template<typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const CSPTR(DATA_TYPE) & source, DeepCopyCacheType & cache);
    template<typename DATA_TYPE>
    static SPTR(DATA_TYPE) copy(const SPTR(DATA_TYPE) & source, DeepCopyCacheType & cache);
    /** @} */

    /// Fields
    FieldMapType m_fields;

    /// Mutex to protect object access.
    mutable core::mt::ReadWriteMutex m_mutex;
};

template<typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const CSPTR(DATA_TYPE) & source, DeepCopyCacheType & cache)
{
    return DATA_TYPE::dynamicCast(Object::copy(Object::csptr(source), cache));
}

template<typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const SPTR(DATA_TYPE) & source, DeepCopyCacheType & cache)
{
    return DATA_TYPE::dynamicCast(Object::copy(Object::csptr(source), cache));
}

template<typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const CSPTR(DATA_TYPE) & source)
{
    return DATA_TYPE::dynamicCast(Object::copy(Object::csptr(source)));
}

template<typename DATA_TYPE>
SPTR(DATA_TYPE) Object::copy(const SPTR(DATA_TYPE) & source)
{
    return DATA_TYPE::dynamicCast(Object::copy(Object::csptr(source)));
}

//-----------------------------------------------------------------------------

template<typename DATA_TYPE>
SPTR(DATA_TYPE) Object::getField(const FieldNameType& name) const
{
    Object::sptr field;
    field                  = this->getField(name, field);
    SPTR(DATA_TYPE) result = DATA_TYPE::dynamicCast(field);
    return result;
}

//-----------------------------------------------------------------------------

template<typename DATA_TYPE>
SPTR(DATA_TYPE) Object::getField(const FieldNameType& name, SPTR(DATA_TYPE) defaultValue) const
{
    Object::sptr field = defaultValue;
    field                  = this->getField(name, field);
    SPTR(DATA_TYPE) result = DATA_TYPE::dynamicCast(field);
    return result;
}

//-----------------------------------------------------------------------------

template<typename DATA_TYPE>
SPTR(DATA_TYPE) Object::setDefaultField(const FieldNameType& name, SPTR(DATA_TYPE) defaultValue)
{
    SPTR(DATA_TYPE) result = getField<DATA_TYPE>(name);
    if(!result && defaultValue)
    {
        result = defaultValue;
        this->setField(name, defaultValue);
    }

    return result;
}

} // namespace sight::data
