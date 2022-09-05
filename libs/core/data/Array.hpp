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
#include "data/Exception.hpp"
#include "data/factory/new.hpp"
#include "data/iterator.hpp"
#include "data/Object.hpp"

#include <core/memory/BufferObject.hpp>
#include <core/memory/IBuffered.hpp>
#include <core/Type.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::data
{

/**
 * @brief   Provides a way to manage a view on a multidimensional array.
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
 * use setType(const core::Type& type) and resize(const SizeType& size, bool reallocate) or directly call
 * resize(const SizeType& size, const core::Type& type, bool reallocate).
 *
 * @section Access Buffer access
 *
 * You can access buffer values using at<type>(const std::size_t& offset) or at<type>({x, y, z}) methods. These methods
 * are
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
    const auto dumpLock = array->dump_lock();

    // retrieve the value at index (x, y)
    value = array->at<std::int16_t>({x, y});

    // or you can compute the index like
    const auto size = array->getSize();
    const std::size_t index = x + y*size[0];
    value = array->at<std::int16_t>(index);
   @endcode
 *
 * @subsection iterators iterators
 *
 * To parse the buffer from beginning to end, the iterator can be used.
 *
 * The iteration depends on the given format. The format can be the buffer type ([u]int[8|16|32|64], double, float), but
 * can also be a simple struct like:
 *
 * @code{.cpp}
    struct rgba {
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
        std::uint8_t a;
    };
    @endcode
 *
 * This struct allows to parse the array as an rgba buffer (RGBARGBARGBA....).
 *
 * To get an iterator on the array, use begin<T>() and end<T>() methods.
 *
 * @warning The iterator does not assert that the array type is the same as the given format. It only asserts (in debug)
 * that the iterator does not iterate outside of the buffer bounds).
 *
 * \b Example :
 */
/* *INDENT-OFF* */
/**
 * @code{.cpp}
    Array::sptr array = Array::New();
    array->resize({1920, 1080}, core::Type::INT16);
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
    for (std::size_t y=0 ; y<size[1] ; ++y)
    {
        for (std::size_t x=0 ; x<size[0] ; ++x)
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
class DATA_CLASS_API Array : public Object,
                             public core::memory::IBuffered
{
public:

    SIGHT_DECLARE_CLASS(Array, Object, factory::New<Array>);

    /**
     * @brief Array size type
     */
    typedef std::vector<std::size_t> SizeType;
    /**
     * @brief Offset type
     */
    typedef std::vector<std::size_t> OffsetType;
    /**
     * @brief Index type
     */
    typedef OffsetType IndexType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Array(Object::Key key);

    DATA_API ~Array() override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

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
     * @throw Exception
     */
    DATA_API std::size_t resize(const SizeType& size, const core::Type& type, bool reallocate = true);

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
     * @throw Exception
     */
    DATA_API std::size_t resize(const SizeType& size, bool reallocate = true);

    /**
     * @brief Clear this array.
     * Size and type are reset, buffer is released.
     */
    DATA_API void clear();

    /**
     * @brief Test whether array is empty
     *
     * @return Returns whether array is empty, ie. this->getSize() is an empty
     * vector.
     */
    DATA_API bool empty() const;

    /**
     * @brief Get the size of one element of the array,
     * ie. sizeof type * nbOfComponents
     *
     * @return One array element size in bytes.
     */
    DATA_API std::size_t getElementSizeInBytes() const;

    /**
     * @brief Get the number of elements of type <getType()> in the array
     *
     * @return number of array elements
     */
    DATA_API std::size_t numElements() const;

    /**
     * @brief Getter for the array view size
     *
     * @return  Array view size in bytes.
     */
    DATA_API std::size_t getSizeInBytes() const;

    /**
     * @brief Getter for the array size
     *
     * @return vector of size lengths in each dimension
     */
    DATA_API const SizeType& getSize() const;

    /**
     * @brief Getter for the array strides
     *
     * @return vector of steps in each dimension for array walking
     */
    DATA_API const OffsetType& getStrides() const;

    /**
     * @brief Getter for number of dimensions, ie. getSize().size()
     *
     * @return Array's number of dimensions
     */
    DATA_API std::size_t numDimensions() const;

    /**
     * @brief Set array's buffer ownership
     *
     * @param own New ownership value
     */
    DATA_API void setIsBufferOwner(bool own);

    /**
     * @brief Getter for array's buffer ownership
     *
     * @return Current array buffer ownership
     */
    DATA_API bool getIsBufferOwner() const;

    /**
     * @brief Getter for array's type
     *
     * @return Type of array
     */
    DATA_API core::Type getType() const;

    /**
     * @brief Compute strides for given parameters
     *
     * @param size Array size
     * @param sizeOfType size of a component
     */
    DATA_API static OffsetType computeStrides(SizeType size, std::size_t sizeOfType);

    /// Return buffer object
    ///@{
    core::memory::BufferObject::sptr getBufferObject();
    core::memory::BufferObject::csptr getBufferObject() const;
    ///@}

    /// Set buffer object
    void setBufferObject(const core::memory::BufferObject::sptr& bufferObj);

    /// Exchanges the content of the Array with the content of _source.
    DATA_API void swap(Array::sptr _source);

    template<typename T>
    using iterator = array_iterator<T>;
    template<typename T>
    using const_iterator = array_iterator<const T>;

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item array index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    T& at(const Array::IndexType& id);

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    const T& at(const Array::IndexType& id) const;

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param offset Index of the item in the buffer cast to T
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    T& at(const std::size_t& offset);

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param offset Index of the item in the buffer cast to T
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    const T& at(const std::size_t& offset) const;

    /**
     * @brief Getter for the array buffer
     *
     * @return Array's buffer, if exists, else NULL
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
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
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     */
    DATA_API void setBuffer(
        void* buf,
        bool takeOwnership,
        const Array::SizeType& size,
        const core::Type& type,
        core::memory::BufferAllocationPolicy::sptr policy = core::memory::BufferMallocPolicy::New()
    );

    /**
     * @brief Returns the beginning/end iterators to the array buffer, cast to T
     * @warning Print a warning if T is different from the array type
     * @note These functions lock the buffer
     * @{
     */
    template<typename T>
    iterator<T> begin();
    template<typename T>
    iterator<T> end();
    template<typename T>
    const_iterator<T> begin() const;
    template<typename T>
    const_iterator<T> end() const;
    template<typename T>
    const_iterator<T> cbegin() const;
    template<typename T>
    const_iterator<T> cend() const;
    /// @}

    /**
     * @brief Returns a range of begin/end iterators, especially useful to be used in "for range loops".
     * @{
     */
    template<typename T>
    auto range();
    template<typename T>
    auto range() const;
    template<typename T>
    auto crange() const;
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
        struct rgba {
            std::uint8_t r;
            std::uint8_t g;
            std::uint8_t b;
            std::uint8_t a;
        };
        @endcode
     *
     * This struct allows to parse the array as an rgba buffer (RGBARGBARGBA....).
     *
     * Example :
     * @code{.cpp}
        Array::sptr array = Array::New();
        array->resize({1920, 1080}, core::Type::INT16);
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
    DATA_API iterator<char> begin();
    DATA_API iterator<char> end();
    DATA_API const_iterator<char> begin() const;
    DATA_API const_iterator<char> end() const;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Array& other) const noexcept;
    DATA_API bool operator!=(const Array& other) const noexcept;
    /// @}

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

    /**
     * @brief Retrieves a pointer to the value at the given index.
     *
     * @param id Item array index
     * @return buffer item pointer
     * @{
     */
    DATA_API char* getBufferPtr(const Array::IndexType& id);
    DATA_API const char* getBufferPtr(const Array::IndexType& id) const;
    ///@}

    /**
     * @brief Compute the offset of an element in the buffer.
     * @param id Item array index
     * @return buffer offset
     */
    DATA_API std::size_t getBufferOffset(const Array::IndexType& id) const;

    /// Add a lock on the array in the given vector to prevent from dumping the buffer on the disk
    /// This is needed for IBuffered interface implementation
    DATA_API void dump_lock_impl(std::vector<core::memory::BufferObject::Lock>& locks) const override;

    /// Not implemented
    Array(const Array&);

    Array& operator=(const Array&);

private:

    OffsetType m_strides {0};
    core::Type m_type;
    core::memory::BufferObject::sptr m_bufferObject;
    SizeType m_size;
    bool m_isBufferOwner {true};
};

//-----------------------------------------------------------------------------

inline core::memory::BufferObject::csptr Array::getBufferObject() const
{
    return m_bufferObject;
}

//-----------------------------------------------------------------------------

inline core::memory::BufferObject::sptr Array::getBufferObject()
{
    return m_bufferObject;
}

//-----------------------------------------------------------------------------

inline void Array::setBufferObject(const core::memory::BufferObject::sptr& bufferObj)
{
    m_bufferObject = bufferObj;
}

//------------------------------------------------------------------------------

template<typename T>
inline T& Array::at(const Array::IndexType& id)
{
    const bool isIndexInBounds =
        std::equal(
            id.begin(),
            id.end(),
            m_size.begin(),
            [](const IndexType::value_type& a, const IndexType::value_type& b)
        {
            return a < b;
        });
    SIGHT_THROW_EXCEPTION_IF(Exception("Index out of bounds"), !isIndexInBounds);
    return *reinterpret_cast<T*>(this->getBufferPtr(id));
}

//------------------------------------------------------------------------------

template<typename T>
inline const T& Array::at(const Array::IndexType& id) const
{
    const bool isIndexInBounds =
        std::equal(id.begin(), id.end(), m_size.begin(), std::less<>());
    SIGHT_THROW_EXCEPTION_IF(Exception("Index out of bounds"), !isIndexInBounds);
    return *reinterpret_cast<T*>(this->getBufferPtr(id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T& Array::at(const std::size_t& offset)
{
    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Index out of bounds, " + std::to_string(offset) + " is not in [0-"
            + std::to_string(this->getSizeInBytes() / sizeof(T) - 1) + "]"
        ),
        offset >= this->getSizeInBytes() / sizeof(T)
    );
    return *(reinterpret_cast<T*>(this->getBuffer()) + offset);
}

//------------------------------------------------------------------------------

template<typename T>
inline const T& Array::at(const std::size_t& offset) const
{
    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Index out of bounds, " + std::to_string(offset) + " is not in [0-"
            + std::to_string(this->getSizeInBytes() / sizeof(T) - 1) + "]"
        ),
        offset >= this->getSizeInBytes() / sizeof(T)
    );
    return *(reinterpret_cast<const T*>(this->getBuffer()) + offset);
}

//------------------------------------------------------------------------------

template<typename T>
inline Array::iterator<T> Array::begin()
{
    return iterator<T>(static_cast<typename iterator<T>::pointer>(getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline Array::iterator<T> Array::end()
{
    auto itr = begin<T>();
    itr += static_cast<typename iterator<T>::difference_type>(this->getSizeInBytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline Array::const_iterator<T> Array::begin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer>(getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline Array::const_iterator<T> Array::end() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->getSizeInBytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline Array::const_iterator<T> Array::cbegin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer>(getBuffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline Array::const_iterator<T> Array::cend() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->getSizeInBytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
auto Array::range()
{
    auto b = begin<T>();
    auto e = end<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
auto Array::range() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
auto Array::crange() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

} // namespace sight::data
