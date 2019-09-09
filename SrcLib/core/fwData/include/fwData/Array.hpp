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

    fwCampMakeFriendDataMacro((fwData)(Array))

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

    FWDATA_API virtual ~Array() override;

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
     * @brief Iterator on array buffer
     *
     * Iterate through the buffer and check if the idex is not out of the bounds
     */
    template <class TYPE, bool isConstIterator = true>
    class IteratorBase : public std::iterator<std::random_access_iterator_tag, TYPE>
    {
    public:

        /**
         * For ConstIterator:   define input to be a const Array*
         * For Iterator: define input to be a Array*
         */
        typedef typename std::conditional<isConstIterator, const Array*, Array*>::type ArrayType;

        /**
         * For ConstIterator:   define buffer type to be a const TYPE*
         * For Iterator: define buffer type to be a TYPE*
         */
        typedef typename std::conditional<isConstIterator, const TYPE*, TYPE*>::type BufferType;

        /**
         * For const_iterator:   define ValueReferenceType to be a   const TYPE&
         * For regular iterator: define ValueReferenceType to be a   TYPE&
         */
        typedef typename std::conditional<isConstIterator, const TYPE&, TYPE&>::type ValueReferenceType;

        /// Constructor
        IteratorBase(ArrayType array);
        /// Copy constructor
        IteratorBase(const IteratorBase<TYPE, false>& other);
        /// Desttructor
        ~IteratorBase();

        /// Comparison operators
        bool operator==(const IteratorBase& other) const;
        bool operator!=(const IteratorBase& other) const;

        /// Increment/Decrement operators
        IteratorBase& operator++();
        IteratorBase operator++(int);
        IteratorBase& operator+(std::ptrdiff_t index);
        IteratorBase& operator+=(std::ptrdiff_t index);
        IteratorBase& operator--();
        IteratorBase operator--(int);
        IteratorBase& operator-(std::ptrdiff_t index);
        IteratorBase& operator-=(std::ptrdiff_t index);

        std::ptrdiff_t operator+(const IteratorBase& other) const;
        std::ptrdiff_t operator-(const IteratorBase& other) const;

        /// Value access operators
        ValueReferenceType operator*();

    private:

        /// allow to create a ConstIterator from an Iterator
        friend class IteratorBase<TYPE, true>;

        BufferType m_pointer{nullptr};
        ::fwMemory::BufferObject::Lock m_lock;
        std::ptrdiff_t m_idx{0};
        const std::ptrdiff_t m_numberOfElements;
    };

    template <typename TYPE>
    using Iterator = IteratorBase<TYPE, false>;
    template <typename TYPE>
    using ConstIterator = IteratorBase<TYPE, true>;

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
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     *
     * @return Buffer value casted to T
     * @warning This method is slow and should not be used intensively
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @throw ::fwData::Exception Index out of bounds
     */
    template< typename T > T& at(const ::fwData::Array::IndexType& id);

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     *
     * @return Buffer value casted to T
     * @warning This method is slow and should not be used intensively
     * @throw ::fwData::Exception The buffer cannot be accessed if the array is not locked
     * @throw ::fwData::Exception Index out of bounds
     */
    template< typename T > T at(const ::fwData::Array::IndexType& id) const;

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
     * @brief Returns the begining/end iterators the array buffer, casted to T
     * @warning Print a warning if T is different to the array type
     * @note Lock the buffer
     * @{
     */
    template< typename T > Iterator<T> begin();
    template< typename T > Iterator<T> end();
    template< typename T > Array::ConstIterator<T> begin() const;
    template< typename T > Array::ConstIterator<T> end() const;
    /// @}
    ///
    /**
     * @brief Returns the begining/end iterators the array buffer, casted to char*
     * @note Lock the buffer
     * @{
     */
    Iterator<char*> begin();
    Iterator<char*> end();

    ConstIterator<char*> begin() const;
    ConstIterator<char*> end() const;
    /// @}

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

    /**
     * @brief Compute offset in buffer
     *
     * @param id Item id
     *
     * @return buffer offset
     */
    FWDATA_API size_t getBufferOffset( const ::fwData::Array::IndexType& id ) const;

    /// Not implemented
    Array( const Array& );

    const Array& operator= ( const Array& );

private:

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
inline Array::Iterator<T> Array::begin()
{
    SLM_WARN_IF("Array is of type '" + m_type.string() + "', but you try get a buffer of type '" +
                ::fwTools::Type::create<T>().string() + "'", m_type != ::fwTools::Type::create<T>());
    return Iterator<T>(this);
}

//------------------------------------------------------------------------------

template< typename T >
inline Array::Iterator<T> Array::end()
{
    SLM_WARN_IF("Array is of type '" + m_type.string() + "', but you try get a buffer of type '" +
                ::fwTools::Type::create<T>().string() + "'", m_type != ::fwTools::Type::create<T>());

    auto itr = Iterator<T>(this);
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes()/sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template< typename T >
inline Array::ConstIterator<T> Array::begin() const
{
    SLM_WARN_IF("Array is of type '" + m_type.string() + "', but you try get a buffer of type '" +
                ::fwTools::Type::create<T>().string() + "'", m_type != ::fwTools::Type::create<T>());
    return ConstIterator<T>(this);
}

//------------------------------------------------------------------------------

template< typename T >
inline Array::ConstIterator<T> Array::end() const
{
    SLM_WARN_IF("Array is of type '" + m_type.string() + "', but you try get a buffer of type '" +
                ::fwTools::Type::create<T>().string() + "'", m_type != ::fwTools::Type::create<T>());

    auto itr = ConstIterator<T>(this);
    itr += static_cast<std::ptrdiff_t>(this->getSizeInBytes()/sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template< typename T >
inline T& Array::at(const ::fwData::Array::IndexType& id)
{
    const bool isIndexInBounds =
        std::equal(id.begin(), id.end(), m_size.begin(),
                   [](const IndexType::value_type& a, const IndexType::value_type& b)
        {
            return a < b;
        });
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Index out of bounds"), !isIndexInBounds);
    return *reinterpret_cast<T*>(this->getBufferPtr(id));
}

//------------------------------------------------------------------------------

template< typename T >
inline T Array::at(const ::fwData::Array::IndexType& id) const
{
    const bool isIndexInBounds =
        std::equal(id.begin(), id.end(), m_size.begin(),
                   [](const IndexType::value_type& a, const IndexType::value_type& b)
        {
            return a < b;
        });
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Index out of bounds"), !isIndexInBounds);
    return *reinterpret_cast<T*>(this->getBufferPtr(id));
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>::IteratorBase(ArrayType array) :
    m_pointer(static_cast<BufferType>(array->getBuffer())),
    m_lock(array->lock()),
    m_idx(0),
    m_numberOfElements(static_cast<std::ptrdiff_t>(array->getSizeInBytes()/sizeof(TYPE)))
{
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>::IteratorBase(const IteratorBase<TYPE, false>& other) :
    m_pointer(other.m_pointer),
    m_lock(other.m_lock),
    m_idx(other.m_idx),
    m_numberOfElements(other.m_numberOfElements)
{
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>::~IteratorBase()
{
    m_lock.reset();
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline bool Array::IteratorBase<TYPE, isConst>::operator==(const IteratorBase& other) const
{
    return m_pointer == other.m_pointer;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline bool Array::IteratorBase<TYPE, isConst>::operator!=(const IteratorBase& other) const
{
    return m_pointer != other.m_pointer;
}

//------------------------------------------------------------------------------

template <typename TYPE, bool isConst>
inline typename Array::IteratorBase<TYPE, isConst>::ValueReferenceType Array::IteratorBase<TYPE, isConst>::operator*()
{
    return *m_pointer;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator++()
{
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    ++m_pointer;
    return *this;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst> Array::IteratorBase<TYPE, isConst>::operator++(int)
{
    IteratorBase tmp(*this);
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    ++m_pointer;
    return tmp;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator+(std::ptrdiff_t index)
{
    m_idx = m_idx + index;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer = m_pointer + index;
    return *this;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator+=(std::ptrdiff_t index)
{
    m_idx += index;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += index;
    return *this;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    --m_pointer;
    return *this;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst> Array::IteratorBase<TYPE, isConst>::operator--(int)
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    IteratorBase tmp(*this);
    --m_pointer;
    return tmp;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator-(std::ptrdiff_t index)
{
    SLM_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                             << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );
    m_idx     = m_idx - index;
    m_pointer = m_pointer - index;
    return *this;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator-=(std::ptrdiff_t index)
{
    SLM_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                             << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );
    m_idx     -= index;
    m_pointer -= index;
    return *this;
}

//-----------------------------------------------------------------------------

template <class TYPE, bool isConst>
std::ptrdiff_t Array::IteratorBase<TYPE, isConst>::operator+(const IteratorBase& other) const
{
    return m_pointer + other.m_pointer;
}

//-----------------------------------------------------------------------------

template <class TYPE, bool isConst>
std::ptrdiff_t Array::IteratorBase<TYPE, isConst>::operator-(const IteratorBase& other) const
{
    return m_pointer - other.m_pointer;
}

//-----------------------------------------------------------------------------

} // namespace fwData
