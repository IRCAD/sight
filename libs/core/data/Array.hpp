/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "data/Exception.hpp"
#include "data/factory/new.hpp"
#include "data/Object.hpp"

#include <core/memory/BufferObject.hpp>
#include <core/memory/IBuffered.hpp>
#include <core/tools/Type.hpp>

SIGHT_DECLARE_DATA_REFLECTION((sight)(data)(Array));

namespace sight::data
{

/**
 * @brief   Provides a way to manage a view on a multidimentionnal array.
 *
 * If the array owns its buffer, it will perform the allocation, reallocation,
 * destruction of the buffer. Else, this class will provide an array "view" of the buffer.
 *
 * The array can be multi-dimensional, the number of dimensions is defined by the number of elements in the size vector
 * (getSize()).
 *
 * @section Usage Usage
 *
 * @subsection Allocation Allocation
 *
 * The array buffer is allocated using the resize() method.
 * You can get the allocated size using getSizeInBytes().
 *
 * @warning The allocated size can be different from the array size: it can happen if you called resize(..., false).
 * It may be useful when you don't want to reallocate the image too often, but you need to be sure to allocate enough
 * memory.
 *
 * To resize the array, you must define the Type ([u]int[8|16|32|64], double, float) and the size of the buffer. You can
 * use setType(const core::tools::Type& type) and resize(const SizeType& size, bool reallocate) or directly call
 * resize(const SizeType& size, const core::tools::Type& type, bool reallocate).
 *
 * @section Access Buffer access
 *
 * You can access buffer values using at<type>(const size_t& offset) or at<type>({x, y, z}) methods. These methods are
 * slow and should not be used to parse the entire buffer (see iterators).
 *
 * @warning The array must be locked for dump before accessing the buffer. It prevents the buffer to be dumped on the
 * disk.
 *
 * \b Example:
 *
 * @code{.cpp}

    // 2D array of std::int16_t

    // prevent the buffer to be dumped on the disk
    const auto dumpLock = array->lock();

    // retrieve the value at index (x, y)
    value = array->at<std::int16_t>({x, y});

    // or you can compute the index like
    const auto size = array->getSize();
    const size_t index = x + y*size[0];
    value = array->at<std::int16_t>(index);
   @endcode
 *
 * @subsection Iterators Iterators
 *
 * To parse the buffer from beginning to end, the iterator can be used.
 *
 * The iteration depends on the given format. The format can be the buffer type ([u]int[8|16|32|64], double, float), but
 * can also be a simple struct like:
 *
 * @code{.cpp}
    struct RGBA {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };
    @endcode
 *
 * This struct allows to parse the array as an RGBA buffer (RGBARGBARGBA....).
 *
 * To get an iterator on the array, use begin<FORMAT>() and end<FORMAT>() methods.
 *
 * @warning The iterator does not assert that the array type is the same as the given format. It only asserts (in debug)
 * that the iterator does not iterate outside of the buffer bounds).
 *
 * \b Example :
 */
/* *INDENT-OFF* */
/**
 * @code{.cpp}
    data::Array::sptr array = data::Array::New();
    array->resize({1920, 1080}, core::tools::Type::s_INT16);
    auto iter          = array->begin<std::int16_t>();
    const auto iterEnd = array->end<std::int16_t>();

    for (; iter != iterEnd; ++iter)
    {
        (*iter) = value;
    }
   @endcode
 *
 * @note If you need to know (x, y) indices, you can parse the array looping from the last dimension to the first, like:
 * @code{.cpp}
    auto iter = array->begin<std::int16_t>();

    const auto size = array->getSize();
    for (size_t y=0 ; y<size[1] ; ++y)
    {
        for (size_t x=0 ; x<size[0] ; ++x)
        {
            // do something with x and y ....

            // retrieve the value
            *iter = value;

            // increment iterator
            ++iter;
        }
    }
   @endcode
 */
/* *INDENT-ON* */
class DATA_CLASS_API Array : public data::Object,
                             public core::memory::IBuffered
{
public:

    SIGHT_DECLARE_CLASS(Array, data::Object, data::factory::New< Array >)

    SIGHT_MAKE_FRIEND_REFLECTION((sight)(data)(Array))

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

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Array( data::Object::Key key );

    DATA_API ~Array() override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

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
     * @throw data::Exception
     */
    DATA_API virtual size_t resize(const SizeType& size, bool reallocate = true);

    /**
     * @brief Clear this array.
     * Size and type are reset, buffer is released.
     */
    DATA_API virtual void clear();

    /**
     * @brief Test whether array is empty
     *
     * @return Returns whether array is empty, ie. this->getSize() is an empty
     * vector.
     */
    DATA_API virtual bool empty() const;

    /**
     * @brief Get the size of one element of the array,
     * ie. sizeof type * nbOfComponents
     *
     * @return One array elemet size in bytes.
     */
    DATA_API virtual size_t getElementSizeInBytes() const;

    /**
     * @brief Get the number of elements of type <getType()> in the array
     *
     * @return number of array elements
     */
    DATA_API virtual size_t getNumberOfElements() const;

    /**
     * @brief Getter for the array view size
     *
     * @return  Array view size in bytes.
     */
    DATA_API virtual size_t getSizeInBytes() const;

    /**
     * @brief Getter for the array size
     *
     * @return vector of size lengths in each dimension
     */
    DATA_API virtual const SizeType& getSize() const;

    /**
     * @brief Getter for the array strides
     *
     * @return vector of steps in each dimension for array walking
     */
    DATA_API virtual const OffsetType& getStrides() const;

    /**
     * @brief Getter for number of dimensions, ie. getSize().size()
     *
     * @return Array's number of dimensions
     */
    DATA_API virtual size_t getNumberOfDimensions() const;

    /**
     * @brief Set array's buffer ownership
     *
     * @param own New ownership value
     */
    DATA_API virtual void setIsBufferOwner(const bool own);

    /**
     * @brief Getter for array's buffer ownership
     *
     * @return Current array buffer ownership
     */
    DATA_API virtual bool getIsBufferOwner() const;

    /**
     * @{
     * @brief Setter for array's type
     *
     * @param type new array type
     */
    DATA_API virtual void setType(const std::string& type);
    DATA_API virtual void setType(const core::tools::Type& type);
    /// @}
    /**
     * @brief Getter for array's type
     *
     * @return Type of array
     */
    DATA_API virtual core::tools::Type getType() const;

    /**
     * @brief Compute offset in buffer
     *
     * @param id Item id
     * @param component Item component id
     * @param sizeOfType size of a component
     *
     * @return buffer offset
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0. Use getBufferOffset( const data::Array::IndexType& id, size_t sizeOfType )
     */
    [[deprecated("will be removed in sight 22.0,"
                 "use getBufferOffset( const data::Array::IndexType& id, size_t sizeOfType )")]]
    DATA_API size_t getBufferOffset( const data::Array::IndexType& id, size_t component, size_t sizeOfType) const;

    /**
     * @brief Compute strides for given parameters
     *
     * @param size Array size
     * @param sizeOfType size of a component
     */
    DATA_API static OffsetType computeStrides( SizeType size, size_t sizeOfType );

    /// Return buffer object
    core::memory::BufferObject::sptr getBufferObject() const;

    /// Set buffer object
    void setBufferObject (const core::memory::BufferObject::sptr& bufferObj);

    /// Exchanges the content of the Array with the content of _source.
    DATA_API void swap( Array::sptr _source );

    // -----------------------------------
    // New Array API
    // -----------------------------------

    /**
     * @brief Iterator on array buffer
     *
     * Iterate through the buffer and check if the index is not out of the bounds
     */
    template <class TYPE, bool isConstIterator = true>
    class IteratorBase
    {
    public:

        /**
         * For ConstIterator:   define input to be a const Array*
         * For Iterator: define input to be a Array*
         */
        typedef typename std::conditional<isConstIterator, const Array*, Array*>::type ArrayType;

        /**
         * @name Typedefs required by std::iterator_traits
         * @{
         */
        /**
         * For ConstIterator:   define buffer type to be a const TYPE*
         * For Iterator: define buffer type to be a TYPE*
         */
        typedef typename std::conditional<isConstIterator, const TYPE*, TYPE*>::type pointer;

        /**
         * For const_iterator:   define value_type to be a   const TYPE
         * For regular iterator: define value_type to be a   TYPE
         */
        typedef typename std::conditional<isConstIterator, const TYPE, TYPE>::type value_type;

        /**
         * For const_iterator:   define reference to be a   const TYPE&
         * For regular iterator: define reference to be a   TYPE&
         */
        typedef typename std::conditional<isConstIterator, const TYPE&, TYPE&>::type reference;

        /// Define difference type
        typedef std::ptrdiff_t difference_type;

        /// define the category of the iterator.
        typedef std::random_access_iterator_tag iterator_category;
        /// @}

        /// Constructor
        IteratorBase();
        IteratorBase(ArrayType array);
        /// Copy constructor
        IteratorBase(const IteratorBase<TYPE, false>& other);
        IteratorBase(const IteratorBase<TYPE, true>& other);
        /// Destructor
        ~IteratorBase();

        IteratorBase& operator=(const IteratorBase& other) = default;
        IteratorBase& operator=(IteratorBase&& other)      = default;

        /// Comparison operators
        bool operator==(const IteratorBase& other) const;
        bool operator!=(const IteratorBase& other) const;

        /// Increment/Decrement operators
        IteratorBase& operator++();
        IteratorBase operator++(int);
        IteratorBase operator+(difference_type index) const;
        IteratorBase& operator+=(difference_type index);
        IteratorBase& operator--();
        IteratorBase operator--(int);
        IteratorBase operator-(difference_type index) const;
        IteratorBase& operator-=(difference_type index);

        difference_type operator+(const IteratorBase& other) const;
        difference_type operator-(const IteratorBase& other) const;

        /// Value access operators
        reference operator*() const;
        value_type* operator->() const;

    private:

        /// allow to create a ConstIterator from an Iterator
        friend class IteratorBase<TYPE, true>;

        core::memory::BufferObject::Lock m_lock;
        pointer m_pointer{nullptr};
        difference_type m_idx{0};
        difference_type m_numberOfElements{0};
    };

    template <typename TYPE>
    using Iterator = IteratorBase<TYPE, false>;
    template <typename TYPE>
    using ConstIterator = IteratorBase<TYPE, true>;

    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will allocate a buffer and take ownership of it.
     * If the combination of type and size parameters does not match the size of the previously allocated buffer
     * anymore, a reallocation is needed.
     * In this case :
     *  - if reallocate is true and if the Array does not own the buffer, an exception is thrown
     *  - else if reallocate is false, the array will update the view's information
     *  - else, the reallocation is performed.
     *
     * @param size           New size of the array or the view.
     * @param type           New type of the array or the view
     * @param reallocate     If true, allow buffer reallocation
     *
     * @return return the size of the array view
     *
     * @throw data::Exception
     */
    DATA_API size_t resize(const SizeType& size, const core::tools::Type& type, bool reallocate = true);

    /**
     * @brief Return a lock on the array to prevent from dumping the buffer on the disk.
     *
     * When the buffer is dumped, the memory is released and the buffer will not be accessible. When lock() is called,
     * the buffer is restored from the disk if it was dumped and as long as the core::memory::BufferObject::Lock is
     * maintained, the buffer will not be dumped.
     *
     * An exception will be raised  if you try to access while the array is not locked.
     */
    [[nodiscard]] DATA_API core::memory::BufferObject::Lock lock() const;

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item array index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock()).
     * @throw data::Exception Index out of bounds
     */
    template< typename T > T& at(const data::Array::IndexType& id);

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock()).
     * @throw data::Exception Index out of bounds
     */
    template< typename T > const T& at(const data::Array::IndexType& id) const;

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param offset Index of the item in the buffer cast to T
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock()).
     * @throw data::Exception Index out of bounds
     */
    template< typename T > T& at(const size_t& offset);

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param offset Index of the item in the buffer cast to T
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock()).
     * @throw data::Exception Index out of bounds
     */
    template< typename T > const T& at(const size_t& offset) const;

    /**
     * @brief Getter for the array buffer
     *
     * @return Array's buffer, if exists, else NULL
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock()).
     * @{
     */
    DATA_API void* getBuffer();
    DATA_API const void* getBuffer() const;
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
     * @param policy         If the array takes ownership of the buffer, specifies the buffer allocation policy.
     * @throw data::Exception The buffer cannot be accessed if the array is not locked (see lock()).
     */
    DATA_API void setBuffer(
        void* buf,
        bool takeOwnership,
        const data::Array::SizeType& size,
        const core::tools::Type& type,
        core::memory::BufferAllocationPolicy::sptr policy = core::memory::BufferMallocPolicy::New()
        );

    /**
     * @brief Returns the beginning/end iterators to the array buffer, cast to T
     * @warning Print a warning if T is different from the array type
     * @note These functions lock the buffer
     * @{
     */
    template< typename T > Iterator<T> begin();
    template< typename T > Iterator<T> end();
    template< typename T > ConstIterator<T> begin() const;
    template< typename T > ConstIterator<T> end() const;
    /// @}
    ///
    /**
     * @brief Returns the beginning/end iterators to the array buffer, cast to char*
     * @note These functions lock the buffer
     *
     * The iteration depends of the given format. The format can be the buffer type ([u]int[8|16|32|64], double, float),
     * but
     * can also be a simple struct like:
     *
     * @code{.cpp}
        struct RGBA {
            std::uint8_t r;
            std::uint8_t g;
            std::uint8_t b;
            std::uint8_t a;
        };
        @endcode
     *
     * This struct allows to parse the array as an RGBA buffer (RGBARGBARGBA....).
     *
     * Example :
     * @code{.cpp}
        data::Array::sptr array = data::Array::New();
        array->resize({1920, 1080}, core::tools::Type::s_INT16);
        auto iter          = array->begin<std::int16_t>();
        const auto iterEnd = array->end<std::int16_t>();

        for (; iter != iterEnd; ++iter)
        {
     * iter = value;
        }
       @endcode
     * @warning The iterator does not assert that the array type is the same as the given format. It only asserts (in
     * debug) that the iterator does not iterate outside of the buffer bounds).
     * @note These functions lock the buffer for dump (see lock()).
     * @{
     */
    DATA_API Iterator<char> begin();
    DATA_API Iterator<char> end();
    DATA_API ConstIterator<char> begin() const;
    DATA_API ConstIterator<char> end() const;
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
     * removed in sight 22.0. Use resize(const core::tools::Type& type, const SizeType& size, bool reallocate = false).
     *
     * @throw data::Exception
     */
    [[deprecated("will be removed in sight 22.0,"
                 " use resize(const core::tools::Type& type, const SizeType& size, bool reallocate = false)")]]
    DATA_API virtual size_t resize(const core::tools::Type& type, const SizeType& size,
                                   size_t nbOfComponents,
                                   bool reallocate = false);

    /**
     * @brief  Aliases to the resize method
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0. Use resize(const core::tools::Type& type, const SizeType& size, bool reallocate = false).
     * @{
     */
    [[deprecated("will be removed in sight 22.0,"
                 " use resize(const core::tools::Type& type, const SizeType& size, bool reallocate = false) ")]]
    DATA_API virtual size_t resize(const std::string& type, const SizeType& size,
                                   size_t nbOfComponents,
                                   bool reallocate = false);
    [[deprecated("will be removed in sight 22.0,"
                 " use resize(const core::tools::Type& type, const SizeType& size, bool reallocate = false)")]]
    DATA_API virtual size_t resize(const SizeType& size, size_t nbOfComponents,
                                   bool reallocate = false);
    /// @}

    /**
     * @brief  Temporary method to resize an image's array.
     * @warning This method will be removed with the deprecate API 22.0, it is used to keep the old API of Image
     */
    DATA_API virtual size_t resizeTMP(const core::tools::Type& type, const SizeType& size, size_t nbOfComponents);

    /**
     * @brief  Temporary method to resize a mesh's array.
     * @warning This method will be removed with the deprecate API 22.0, it is used to keep the old API of Mesh
     */
    DATA_API virtual size_t resizeTMP(const SizeType& size, size_t nbOfComponents);

    /**
     * @brief Setter for array's number of components
     * If the array has a buffer and owns it, the buffer will be reallocated
     *
     * @param nb number of components
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0
     */
    [[deprecated("will be removed in sight 22.0, increase array dimension instead.")]]
    DATA_API virtual void setNumberOfComponents(size_t nb);

    /**
     * @brief Getter for number of components
     *
     * @return Array's number of components
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0
     */
    [[deprecated("will be removed in sight 22.0, increase array dimension instead.")]]
    DATA_API virtual size_t getNumberOfComponents() const;

protected:

    /**
     * @brief Protected setter for the array buffer.
     *
     * Releases the previous buffer if it is owned by the array.
     *
     * @param buf Buffer to set as Array's buffer
     * @param takeOwnership if true, the Array will manage allocation and destroy the buffer when needed.
     * @param policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    DATA_API void setBuffer(
        void* buf,
        bool takeOwnership                                = false,
        core::memory::BufferAllocationPolicy::sptr policy = core::memory::BufferMallocPolicy::New()
        );

    // To allow locked_ptr to access protected lockBuffer()
    template< class DATATYPE >
    friend class data::mt::locked_ptr;

    /**
     * @brief Add a lock on the array in the given vector to prevent from dumping the buffer on the disk
     *
     * This is needed for IBuffered interface implementation
     */
    DATA_API void lockBuffer(std::vector< core::memory::BufferObject::Lock >& locks) const override;

    /**
     * @brief Compute strides for given parameters
     *
     * @param size Array size
     * @param nbOfComponents number of components
     * @param sizeOfType size of a component
     * @deprecated Component attribute is deprecated, increase array dimension instead of using component, it will be
     * removed in sight 22.0. Use computeStrides( SizeType size, size_t sizeOfType )
     */
    [[deprecated("will be removed in sight 22.0, use computeStrides( SizeType size, size_t sizeOfType )")]]
    DATA_API static OffsetType computeStrides( SizeType size, size_t nbOfComponents,
                                               size_t sizeOfType );

    /**
     * @brief Retrieves a pointer to the value at the given index.
     *
     * @param id Item array index
     * @return buffer item pointer
     * @{
     */
    DATA_API char* getBufferPtr( const data::Array::IndexType& id);
    DATA_API const char* getBufferPtr( const data::Array::IndexType& id) const;
    ///@}

    /**
     * @brief Compute the offset of an element in the buffer.
     * @param id Item array index
     * @return buffer offset
     */
    DATA_API size_t getBufferOffset( const data::Array::IndexType& id ) const;

    /// Not implemented
    Array( const Array& );

    const Array& operator= ( const Array& );

private:

    OffsetType m_strides;
    core::tools::Type m_type;
    core::memory::BufferObject::sptr m_bufferObject;
    SizeType m_size;
    size_t m_nbOfComponents;
    bool m_isBufferOwner;

};

//-----------------------------------------------------------------------------

inline core::memory::BufferObject::sptr Array::getBufferObject () const
{
    return m_bufferObject;
}

//-----------------------------------------------------------------------------

inline void Array::setBufferObject (const core::memory::BufferObject::sptr& bufferObj)
{
    m_bufferObject = bufferObj;
}

//------------------------------------------------------------------------------

template< typename T >
inline Array::Iterator<T> Array::begin()
{
    return Iterator<T>(this);
}

//------------------------------------------------------------------------------

template< typename T >
inline Array::Iterator<T> Array::end()
{
    auto itr = Iterator<T>(this);
    itr += static_cast< typename Array::Iterator<T>::difference_type>(this->getSizeInBytes()/sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template< typename T >
inline Array::ConstIterator<T> Array::begin() const
{
    return ConstIterator<T>(this);
}

//------------------------------------------------------------------------------

template< typename T >
inline Array::ConstIterator<T> Array::end() const
{
    auto itr = ConstIterator<T>(this);
    itr += static_cast< typename Array::ConstIterator<T>::difference_type>(this->getSizeInBytes()/sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template< typename T >
inline T& Array::at(const data::Array::IndexType& id)
{
    const bool isIndexInBounds =
        std::equal(id.begin(), id.end(), m_size.begin(),
                   [](const IndexType::value_type& a, const IndexType::value_type& b)
        {
            return a < b;
        });
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Index out of bounds"), !isIndexInBounds);
    return *reinterpret_cast<T*>(this->getBufferPtr(id));
}

//------------------------------------------------------------------------------

template< typename T >
inline const T& Array::at(const data::Array::IndexType& id) const
{
    const bool isIndexInBounds =
        std::equal(id.begin(), id.end(), m_size.begin(), std::less<IndexType::value_type>());
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Index out of bounds"), !isIndexInBounds);
    return *reinterpret_cast<T*>(this->getBufferPtr(id));
}

//------------------------------------------------------------------------------

template< typename T >
inline T& Array::at(const size_t& offset)
{
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Index out of bounds, " + std::to_string(offset) + " is not in [0-"
                                             + std::to_string(this->getSizeInBytes()/sizeof(T)-1) + "]"),
                             offset >= this->getSizeInBytes()/sizeof(T));
    return *(reinterpret_cast<T*>(this->getBuffer()) + offset);
}

//------------------------------------------------------------------------------

template< typename T >
inline const T& Array::at(const size_t& offset) const
{
    SIGHT_THROW_EXCEPTION_IF(data::Exception("Index out of bounds, " + std::to_string(offset) + " is not in [0-"
                                             + std::to_string(this->getSizeInBytes()/sizeof(T)-1) + "]"),
                             offset >= this->getSizeInBytes()/sizeof(T));
    return *(reinterpret_cast<const T*>(this->getBuffer()) + offset);
}

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>::IteratorBase()
{

}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>::IteratorBase(ArrayType array) :
    m_lock(array->lock()),
    m_pointer(static_cast<pointer>(array->getBuffer())),
    m_idx(0),
    m_numberOfElements(static_cast<difference_type>(array->getSizeInBytes()/sizeof(TYPE)))
{
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>::IteratorBase(const IteratorBase<TYPE, false>& other) :
    m_lock(other.m_lock),
    m_pointer(other.m_pointer),
    m_idx(other.m_idx),
    m_numberOfElements(other.m_numberOfElements)
{
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>::IteratorBase(const IteratorBase<TYPE, true>& other) :
    m_lock(other.m_lock),
    m_pointer(other.m_pointer),
    m_idx(other.m_idx),
    m_numberOfElements(other.m_numberOfElements)
{
    static_assert(isConst == true, "Cannot convert const Iterator to not const Iterator.");
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
inline typename Array::IteratorBase<TYPE, isConst>::reference Array::IteratorBase<TYPE, isConst>::operator*() const
{
    SIGHT_ASSERT("Iterator needs to be initialized", m_pointer);
    return *m_pointer;
}

//------------------------------------------------------------------------------

template <typename TYPE, bool isConst>
inline typename Array::IteratorBase<TYPE, isConst>::value_type* Array::IteratorBase<TYPE, isConst>::operator->() const
{
    return m_pointer;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator++()
{
    ++m_idx;
    SIGHT_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
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
    SIGHT_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
                 m_idx <= m_numberOfElements );
    ++m_pointer;
    return tmp;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst> Array::IteratorBase<TYPE, isConst>::operator+(difference_type index) const
{
    IteratorBase tmp(*this);
    tmp += index;
    return tmp;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator+=(difference_type index)
{
    m_idx += index;
    SIGHT_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
                 m_idx <= m_numberOfElements );
    m_pointer += index;
    return *this;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator--()
{
    SIGHT_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    --m_pointer;
    return *this;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst> Array::IteratorBase<TYPE, isConst>::operator--(int)
{
    SIGHT_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    IteratorBase tmp(*this);
    --m_pointer;
    return tmp;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst> Array::IteratorBase<TYPE, isConst>::operator-(difference_type index) const
{
    IteratorBase tmp(*this);
    tmp -= index;
    return tmp;
}

//------------------------------------------------------------------------------

template <class TYPE, bool isConst>
inline Array::IteratorBase<TYPE, isConst>& Array::IteratorBase<TYPE, isConst>::operator-=(difference_type index)
{
    SIGHT_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                               << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );
    m_idx     -= index;
    m_pointer -= index;
    return *this;
}

//-----------------------------------------------------------------------------

template <class TYPE, bool isConst>
typename Array::IteratorBase<TYPE, isConst>::difference_type Array::IteratorBase<TYPE, isConst>::operator+(
    const IteratorBase& other) const
{
    return m_pointer + other.m_pointer;
}

//-----------------------------------------------------------------------------

template <class TYPE, bool isConst>
typename Array::IteratorBase<TYPE, isConst>::difference_type Array::IteratorBase<TYPE, isConst>::operator-(
    const IteratorBase& other) const
{
    return m_pointer - other.m_pointer;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
