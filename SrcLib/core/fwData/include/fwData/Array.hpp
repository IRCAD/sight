/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwData/config.hpp"
#include "fwData/Exception.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <fwMemory/BufferObject.hpp>

#include <fwTools/Type.hpp>

fwCampAutoDeclareDataMacro((fwData)(Array), FWDATA_API);

namespace fwData
{

/**
 * @brief   Provides a way to manage a view on a multidimentionnal array.
 *
 * If the array own his buffer, it will perform the allocation, reallocation,
 * destruction of the buffer. Else, this class will provide an array "view" of the
 * buffer
 *
 */
class FWDATA_CLASS_API Array : public ::fwData::Object
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Array)(::fwData::Object), (()), ::fwData::factory::New< Array >);

    fwCampMakeFriendDataMacro((fwData)(Array));

    /**
     * @brief Array size type
     */
    typedef std::vector<size_t> SizeType;
    /**
     * @brief Offset type
     */
    typedef std::vector<size_t> OffsetType;
    /**
     * @brief Index type
     */
    typedef OffsetType IndexType;

    /*
     * public API
     */

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Array( ::fwData::Object::Key key );

    FWDATA_API virtual ~Array();

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will allocate a buffer and take it ownership,
     * the type of the array is not changed.
     *
     * If the combination of type and size  parameters do not match anymore the size of the previously allocated
     * buffer, a reallocation is needed.
     * In this case :
     *  - if reallocate is true and if the Array do not own the buffer, an exception is thrown
     *  - else if reallocate is false, the array will update the view informations
     *  - else, the reallocation is performed.
     *
     * @param size           Size of the array view
     * @param reallocate     If true, allow buffer reallocation
     *
     * @return return the size of the array view
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API virtual size_t resize(const SizeType& size, bool reallocate = false);

    /**
     * @brief Clear this array.
     * Size and type are reset, buffer is released.
     */
    FWDATA_API virtual void clear();

    /**
     * @brief Test whether array is empty
     *
     * @return Returns whether array is empty, ie. this->getSize() is an empty
     * vector.
     */
    FWDATA_API virtual bool empty() const;

    /**
     * @brief Get the size of one element of the array,
     * ie. sizeof type * nbOfComponents
     *
     * @return One array elemet size in bytes.
     */
    FWDATA_API virtual size_t getElementSizeInBytes() const;

    /**
     * @brief Get the number of elements of type <getType()> in the array
     *
     * @return number of array elements
     */
    FWDATA_API virtual size_t getNumberOfElements() const;

    /**
     * @brief Getter for the array view size
     *
     * @return  Array view size in bytes.
     */
    FWDATA_API virtual size_t getSizeInBytes() const;

    /**
     * @brief Getter for the array size
     *
     * @return vector of size lengths in each dimension
     */
    FWDATA_API virtual const SizeType& getSize() const;

    /**
     * @brief Getter for the array strides
     *
     * @return vector of steps in each dimension for array walking
     */
    FWDATA_API virtual const OffsetType& getStrides() const;

    /**
     * @brief Getter for number of dimensions, ie. getSize().size()
     *
     * @return Array's number of dimensions
     */
    FWDATA_API virtual size_t getNumberOfDimensions() const;

    /**
     * @brief Set array's buffer ownership
     *
     * @param own New ownership value
     */
    FWDATA_API virtual void setIsBufferOwner(const bool own);

    /**
     * @brief Getter for array's buffer ownership
     *
     * @return Current array buffer ownership
     */
    FWDATA_API virtual bool getIsBufferOwner() const;

    /**
     * @brief Setter for array's type
     *
     * @param type new array type
     */
    FWDATA_API virtual void setType(const std::string& type);
    FWDATA_API virtual void setType(const ::fwTools::Type& type);

    /**
     * @brief Getter for array's type
     *
     * @return Type of array
     */
    FWDATA_API virtual ::fwTools::Type getType() const;

    /**
     * @brief Compute offset in buffer
     *
     * @param id Item id
     *
     * @return buffer offset
     */
    FWDATA_API size_t getBufferOffset( const ::fwData::Array::IndexType& id ) const;

    /**
     * @brief Compute strides for given parameters
     *
     * @param size Array size
     * @param sizeOfType size of a component
     */
    FWDATA_API static OffsetType computeStrides( SizeType size, size_t sizeOfType );

    ::fwMemory::BufferObject::sptr getBufferObject () const;

    void setBufferObject (const ::fwMemory::BufferObject::sptr& val);

    /// Exchanges the content of the Array with the content of _source.
    FWDATA_API void swap( Array::sptr _source );

    // -----------------------------------
    // New Array API
    // -----------------------------------

    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will allocate a buffer and take it ownership.
     *
     * If the combination of type and size parameters do not match anymore the size of the previously allocated buffer,
     * a reallocation is needed.
     * In this case :
     *  - if reallocate is true and if the Array do not own the buffer, an exception is thrown
     *  - else if reallocate is false, the array will update the view informations
     *  - else, the reallocation is performed.
     *
     * @param size           Size of the array view
     * @param type           Type of the array view
     * @param reallocate     If true, allow buffer reallocation
     *
     * @return return the size of the array view
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API virtual size_t resize(const SizeType& size, const ::fwTools::Type& type, bool reallocate = false);

    /**
     * @brief Return a lock on the array to prevent to dump the buffer on the disk
     *
     * The buffer cannot be accessed if the array is not locked
     */
    FWDATA_API ::fwMemory::BufferObject::Lock lock() const;

    /**
     * @brief Setter for one item components of the array
     *
     * @param id Item id
     * @param value Valid buffer of elements of type \<m_type\> with a length equal to \<m_nbOfComponents\> to be copied
     * to array 'id'
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     */
    FWDATA_API virtual void setItem(const ::fwData::Array::IndexType& id, const void* value);

    /**
     * @brief Getter for a buffer item. pointer to the requested item in the buffer
     *
     * @param id Item id
     *
     * @return Pointer to the requested item in the buffer
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     */
    FWDATA_API virtual void* getItem(const ::fwData::Array::IndexType& id);

    /**
     * @brief Typed version of getItem
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     *
     * @return Array buffer pointer casted to T
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     */
    template< typename T > T* getItem(const ::fwData::Array::IndexType& id);

    /**
     * @brief Copies the data into the buffer pointed by <value>
     *
     * @param id Item id
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @param[out] value Buffer to write into
     */
    FWDATA_API virtual void getItem(const ::fwData::Array::IndexType& id, void* value) const;

    /**
     * @brief Getter for the array buffer
     *
     * @return Array's buffer, if exists, else NULL
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @{
     */
    FWDATA_API virtual void* getBuffer();
    FWDATA_API virtual const void* getBuffer() const;
    ///@}

    /**
     * @brief Setter for the array buffer.
     *
     * An existing buffer will be released if the array own it.
     *
     * @param buf            Buffer to set as Array's buffer
     * @param takeOwnership  if true, the Array will manage allocation and destroy the buffer when needed.
     * @param size           Size of the array view
     * @param type           Type of the array view
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @param policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    FWDATA_API void setBuffer(
        void* buf,
        bool takeOwnership,
        const ::fwData::Array::SizeType& size,
        const ::fwTools::Type& type,
        ::fwMemory::BufferAllocationPolicy::sptr policy = ::fwMemory::BufferMallocPolicy::New()
        );

    /**
     * Returns the begining/end of the buffer interpreted as a char buffer
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @{
     */
    FWDATA_API virtual char* begin();
    FWDATA_API virtual char* end();
    FWDATA_API virtual const char* begin() const;
    FWDATA_API virtual const char* end() const;
    ///@}

    /** Returns the begining/end of the buffer, casted to T
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @{
     */
    template< typename T > T* begin();
    template< typename T > T* end();
    /// @}
    /**
     * @brief Get a pointer to the value described by given parameters
     *
     * @param id Item id
     *
     * @return buffer item pointer
     * @{
     */
    FWDATA_API char* getBufferPtr( const ::fwData::Array::IndexType& id);
    FWDATA_API const char* getBufferPtr( const ::fwData::Array::IndexType& id) const;
    ///@}

    //-----------------------------------------------------
    // Deprecated API
    // ----------------------------------------------------

    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will
     * allocate a buffer and take it ownership.
     *
     * If the combination of type, size and components parameters do not match anymore the size of the previously
     * allocated buffer, a reallocation is needed.
     * In this case :
     *  - if reallocate is true and if the Array do not own the buffer, an exception is thrown
     *  - else if reallocate is false, the array will update the view informations
     *  - else, the reallocation is performed.
     *
     * @param type           Type of the array view
     * @param size           Size of the array view
     * @param nbOfComponents Number of components of the array view, Min value : 1
     * @param reallocate     If true, allow buffer reallocation
     *
     * @return return the size of the array view
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0. Use resize(const ::fwTools::Type& type, const SizeType& size, bool reallocate = false).
     *
     * @throw ::fwData::Exception
     */
    FWDATA_API virtual size_t resize(const ::fwTools::Type& type, const SizeType& size, size_t nbOfComponents,
                                     bool reallocate = false);

    /**
     * @brief  Aliases to the resize method
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0. Use resize(const ::fwTools::Type& type, const SizeType& size, bool reallocate = false).
     * @{
     */
    FWDATA_API virtual size_t resize(const std::string& type, const SizeType& size, size_t nbOfComponents,
                                     bool reallocate = false);
    FWDATA_API virtual size_t resize(const SizeType& size, size_t nbOfComponents, bool reallocate = false);
    /// @}

    /**
     * @brief Setter for array's number of components
     * If the array has a buffer and owns it, the buffer will be reallocated
     *
     * @param nb number of components
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0
     */
    FWDATA_API virtual void setNumberOfComponents(size_t nb);

    /**
     * @brief Getter for number of components
     *
     * @return Array's number of components
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0
     */
    FWDATA_API virtual size_t getNumberOfComponents() const;

    /**
     * @brief Compute offset in buffer
     *
     * @param id Item id
     * @param component Item component id
     * @param sizeOfType size of a component
     *
     * @return buffer offset
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0. Use getBufferOffset( const ::fwData::Array::IndexType& id, size_t sizeOfType )
     */
    FWDATA_API size_t getBufferOffset( const ::fwData::Array::IndexType& id, size_t component, size_t sizeOfType) const;

    /**
     * @brief Compute strides for given parameters
     *
     * @param size Array size
     * @param nbOfComponents number of components
     * @param sizeOfType size of a component
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0. Use computeStrides( SizeType size, size_t sizeOfType )
     */
    FWDATA_API static OffsetType computeStrides( SizeType size, size_t nbOfComponents, size_t sizeOfType );

protected:

    /**
     * @brief Protected setter for the array buffer.
     * An existing buffer will be released if the array own it.
     *
     * @param buf Buffer to set as Array's buffer
     * @param takeOwnership if true, the Array will manage allocation and destroy the buffer when needed.
     * @param policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    FWDATA_API virtual void setBuffer(
        void* buf,
        bool takeOwnership                              = false,
        ::fwMemory::BufferAllocationPolicy::sptr policy = ::fwMemory::BufferMallocPolicy::New()
        );

    /// Not implemented
    Array( const Array& );

    const Array& operator= ( const Array& );

    OffsetType m_strides;
    ::fwTools::Type m_type;
    ::fwMemory::BufferObject::sptr m_bufferObject;
    SizeType m_size;
    size_t m_nbOfComponents;
    bool m_isBufferOwner;

};

//-----------------------------------------------------------------------------

inline ::fwMemory::BufferObject::sptr Array::getBufferObject () const
{
    return m_bufferObject;
}

//-----------------------------------------------------------------------------

inline void Array::setBufferObject (const ::fwMemory::BufferObject::sptr& val)
{
    m_bufferObject = val;
}

//------------------------------------------------------------------------------

template< typename T >
T* Array::begin()
{
    return static_cast<T*>(this->getBuffer());
}

//------------------------------------------------------------------------------

template< typename T >
T* Array::end()
{
    return reinterpret_cast<T*> (static_cast<char*>(this->getBuffer()) + this->getSizeInBytes());
}

//------------------------------------------------------------------------------

template< typename T >
T* Array::getItem(const ::fwData::Array::IndexType& id)
{
    return static_cast<T*> (this->getItem(id));
}

//-----------------------------------------------------------------------------

} // namespace fwData
