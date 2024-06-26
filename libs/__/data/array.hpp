/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/exception.hpp"
#include "data/factory/new.hpp"
#include "data/iterator.hpp"
#include "data/object.hpp"

#include <core/memory/buffer_object.hpp>
#include <core/memory/buffered.hpp>
#include <core/type.hpp>

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
 * (size()).
 *
 * @section Usage Usage
 *
 * @subsection Allocation Allocation
 *
 * The array buffer is allocated using the resize() method.
 * You can get the allocated size using size_in_bytes().
 *
 * @warning The allocated size can be different from the array size: it can happen if you called resize(..., false).
 * It may be useful when you don't want to reallocate the image too often, but you need to be sure to allocate enough
 * memory.
 *
 * To resize the array, you must define the Type ([u]int[8|16|32|64], double, float) and the size of the buffer. You can
 * use setType(const core::type& type) and resize(const size_t& size, bool reallocate) or directly call
 * resize(const size_t& size, const core::type& type, bool reallocate).
 *
 * @section Access Buffer access
 *
 * You can access buffer values using at<type>(const std::size_t& offset) or at<type>({x, y, z}) methods. These methods
 * are slow and should not be used to parse the entire buffer (see iterators).
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
    const auto size = array->size();
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
 * @warning The iterator does not assert that the array type is not the same as the given format. It only asserts (in
 * debug)
 * that the iterator does iterate outside of the buffer bounds).
 *
 * \b Example :
 */
/* *INDENT-OFF* */
/**
 * @code{.cpp}
    array::sptr array = array::New();
    array->resize({1920, 1080}, core::type::INT16);
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

    const auto size = array->size();
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
class SIGHT_DATA_CLASS_API array final : public object,
                                         public core::memory::buffered
{
public:

    SIGHT_DECLARE_CLASS(array, object);

    /**
     * @brief array size type
     */
    using size_t = std::vector<std::size_t>;
    /**
     * @brief Offset type
     */
    using offset_t = std::vector<std::size_t>;
    /**
     * @brief Index type
     */
    using index_t = offset_t;

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API array();

    SIGHT_DATA_API ~array() override;

    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will allocate a buffer and take ownership of it.
     * If the combination of type and size parameters does not match the size of the previously allocated buffer
     * anymore, a reallocation is needed.
     * In this case :
     *  - if reallocate is true and if the array does not own the buffer, an exception is thrown
     *  - else if reallocate is false, the array will update the view's information
     *  - else, the reallocation is performed.
     *
     * @param _size           New size of the array or the view.
     * @param _type           New type of the array or the view
     * @param _reallocate     If true, allow buffer reallocation
     *
     * @return return the size of the array view
     *
     * @throw Exception
     */
    SIGHT_DATA_API std::size_t resize(const size_t& _size, const core::type& _type, bool _reallocate = true);

    /**
     * @brief Resizes and allocate (if needed) the array.
     *
     * If no buffer is allocated and reallocate is true, this method will allocate a buffer and take it ownership,
     * the type of the array is not changed.
     *
     * If the combination of type and size  parameters do not match anymore the size of the previously allocated
     * buffer, a reallocation is needed.
     * In this case :
     *  - if reallocate is true and if the array do not own the buffer, an exception is thrown
     *  - else if reallocate is false, the array will update the view informations
     *  - else, the reallocation is performed.
     *
     * @param _size           Size of the array view
     * @param _reallocate     If true, allow buffer reallocation
     *
     * @return return the size of the array view
     *
     * @throw Exception
     */
    SIGHT_DATA_API std::size_t resize(const size_t& _size, bool _reallocate = true);

    /**
     * @brief Clear this array.
     * Size and type are reset, buffer is released.
     */
    SIGHT_DATA_API void clear();

    /**
     * @brief Test whether array is empty
     *
     * @return Returns whether array is empty, ie. this->size() is an empty
     * vector.
     */
    SIGHT_DATA_API bool empty() const;

    /**
     * @brief Get the size of one element of the array,
     * ie. sizeof type * nbOfComponents
     *
     * @return One array element size in bytes.
     */
    SIGHT_DATA_API std::size_t element_size_in_bytes() const;

    /**
     * @brief Get the number of elements of type <getType()> in the array
     *
     * @return number of array elements
     */
    SIGHT_DATA_API std::size_t num_elements() const;

    /**
     * @brief Getter for the array view size
     *
     * @return  array view size in bytes.
     */
    SIGHT_DATA_API std::size_t size_in_bytes() const;

    /**
     * @brief Getter for the array size
     *
     * @return vector of size lengths in each dimension
     */
    SIGHT_DATA_API const size_t& size() const;

    /**
     * @brief Getter for the array strides
     *
     * @return vector of steps in each dimension for array walking
     */
    SIGHT_DATA_API const offset_t& get_strides() const;

    /**
     * @brief Getter for number of dimensions, ie. size().size()
     *
     * @return array's number of dimensions
     */
    SIGHT_DATA_API std::size_t num_dimensions() const;

    /**
     * @brief Set array's buffer ownership
     *
     * @param _own New ownership value
     */
    SIGHT_DATA_API void set_is_buffer_owner(bool _own);

    /**
     * @brief Getter for array's buffer ownership
     *
     * @return Current array buffer ownership
     */
    SIGHT_DATA_API bool get_is_buffer_owner() const;

    /**
     * @brief Getter for array's type
     *
     * @return Type of array
     */
    SIGHT_DATA_API core::type type() const;

    /**
     * @brief Compute strides for given parameters
     *
     * @param _size         array size
     * @param _size_of_type size of a component
     */
    SIGHT_DATA_API static offset_t compute_strides(size_t _size, std::size_t _size_of_type);

    /// Return buffer object
    ///@{
    core::memory::buffer_object::sptr get_buffer_object();
    core::memory::buffer_object::csptr get_buffer_object() const;
    ///@}

    /// Set buffer object
    void set_buffer_object(const core::memory::buffer_object::sptr& _buffer_obj);

    /// Exchanges the content of the array with the content of _source.
    SIGHT_DATA_API void swap(array::sptr _source) noexcept;

    template<typename T>
    using iterator = array_iterator<T>;
    template<typename T>
    using const_iterator = array_iterator<const T>;

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param _id Item array index
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    T& at(const array::index_t& _id);

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param _id Item id
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    const T& at(const array::index_t& _id) const;

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param _offset Index of the item in the buffer cast to T
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    T& at(const std::size_t& _offset);

    /**
     * @brief Get the value of an element
     *
     * @tparam T Type in which the pointer will be returned
     * @param _offset Index of the item in the buffer cast to T
     *
     * @return Buffer value cast to T
     * @warning This method is slow and should not be used intensively
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @throw Exception Index out of bounds
     */
    template<typename T>
    const T& at(const std::size_t& _offset) const;

    /**
     * @brief Getter for the array buffer
     *
     * @return array's buffer, if exists, else NULL
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     * @{
     */
    SIGHT_DATA_API void* buffer();
    SIGHT_DATA_API const void* buffer() const;
    ///@}

    /**
     * @brief Setter for the array buffer.
     *
     * An existing buffer will be released if the array own it.
     *
     * @param _buf            Buffer to set as array's buffer
     * @param _take_ownership  if true, the array will manage allocation and destroy the buffer when needed.
     * @param _size           Size of the array view
     * @param _type           Type of the array view
     * @param _policy         If the array takes ownership of the buffer, specifies the buffer allocation policy.
     * @throw Exception The buffer cannot be accessed if the array is not locked (see dump_lock_impl()).
     */
    SIGHT_DATA_API void set_buffer(
        void* _buf,
        bool _take_ownership,
        const array::size_t& _size,
        const core::type& _type,
        core::memory::buffer_allocation_policy::sptr _policy = std::make_shared<core::memory::buffer_malloc_policy>()
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
        array::sptr array = array::New();
        array->resize({1920, 1080}, core::type::INT16);
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
    SIGHT_DATA_API iterator<char> begin();
    SIGHT_DATA_API iterator<char> end();
    SIGHT_DATA_API const_iterator<char> begin() const;
    SIGHT_DATA_API const_iterator<char> end() const;
    /// @}

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const array& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const array& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /**
     * @brief Protected setter for the array buffer.
     *
     * Releases the previous buffer if it is owned by the array.
     *
     * @param _buf Buffer to set as array's buffer
     * @param _take_ownership if true, the array will manage allocation and destroy the buffer when needed.
     * @param _policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    SIGHT_DATA_API void set_buffer(
        void* _buf,
        bool _take_ownership                                 = false,
        core::memory::buffer_allocation_policy::sptr _policy = std::make_shared<core::memory::buffer_malloc_policy>()
    );

    /**
     * @brief Retrieves a pointer to the value at the given index.
     *
     * @param _id Item array index
     * @return buffer item pointer
     * @{
     */
    SIGHT_DATA_API char* get_buffer_ptr(const array::index_t& _id);
    SIGHT_DATA_API const char* get_buffer_ptr(const array::index_t& _id) const;
    ///@}

    /**
     * @brief Compute the offset of an element in the buffer.
     * @param _id Item array index
     * @return buffer offset
     */
    SIGHT_DATA_API std::size_t get_buffer_offset(const array::index_t& _id) const;

    /// Add a lock on the array in the given vector to prevent from dumping the buffer on the disk
    /// This is needed for IBuffered interface implementation
    SIGHT_DATA_API void dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const override;

    /// Not implemented
    array(const array&);

    array& operator=(const array&);

private:

    offset_t m_strides {0};
    core::type m_type;
    core::memory::buffer_object::sptr m_buffer_object;
    size_t m_size;
    bool m_is_buffer_owner {true};
};

//-----------------------------------------------------------------------------

inline core::memory::buffer_object::csptr array::get_buffer_object() const
{
    return m_buffer_object;
}

//-----------------------------------------------------------------------------

inline core::memory::buffer_object::sptr array::get_buffer_object()
{
    return m_buffer_object;
}

//-----------------------------------------------------------------------------

inline void array::set_buffer_object(const core::memory::buffer_object::sptr& _buffer_obj)
{
    m_buffer_object = _buffer_obj;
}

//------------------------------------------------------------------------------

template<typename T>
inline T& array::at(const array::index_t& _id)
{
    const bool is_index_in_bounds =
        std::equal(
            _id.begin(),
            _id.end(),
            m_size.begin(),
            [](const index_t::value_type& _a, const index_t::value_type& _b)
        {
            return _a < _b;
        });
    SIGHT_THROW_EXCEPTION_IF(exception("Index out of bounds"), !is_index_in_bounds);
    return *reinterpret_cast<T*>(this->get_buffer_ptr(_id));
}

//------------------------------------------------------------------------------

template<typename T>
inline const T& array::at(const array::index_t& _id) const
{
    const bool is_index_in_bounds =
        std::equal(_id.begin(), _id.end(), m_size.begin(), std::less<>());
    SIGHT_THROW_EXCEPTION_IF(exception("Index out of bounds"), !is_index_in_bounds);
    return *reinterpret_cast<T*>(this->get_buffer_ptr(_id));
}

//------------------------------------------------------------------------------

template<typename T>
inline T& array::at(const std::size_t& _offset)
{
    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Index out of bounds, " + std::to_string(_offset) + " is not in [0-"
            + std::to_string(this->size_in_bytes() / sizeof(T) - 1) + "]"
        ),
        _offset >= this->size_in_bytes() / sizeof(T)
    );
    return *(reinterpret_cast<T*>(this->buffer()) + _offset);
}

//------------------------------------------------------------------------------

template<typename T>
inline const T& array::at(const std::size_t& _offset) const
{
    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Index out of bounds, " + std::to_string(_offset) + " is not in [0-"
            + std::to_string(this->size_in_bytes() / sizeof(T) - 1) + "]"
        ),
        _offset >= this->size_in_bytes() / sizeof(T)
    );
    return *(reinterpret_cast<const T*>(this->buffer()) + _offset);
}

//------------------------------------------------------------------------------

template<typename T>
inline array::iterator<T> array::begin()
{
    return iterator<T>(static_cast<typename iterator<T>::pointer_t>(buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline array::iterator<T> array::end()
{
    auto itr = begin<T>();
    itr += static_cast<typename iterator<T>::difference_type>(this->size_in_bytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline array::const_iterator<T> array::begin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer_t>(buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline array::const_iterator<T> array::end() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->size_in_bytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
inline array::const_iterator<T> array::cbegin() const
{
    return const_iterator<T>(static_cast<typename const_iterator<T>::pointer_t>(buffer()));
}

//------------------------------------------------------------------------------

template<typename T>
inline array::const_iterator<T> array::cend() const
{
    auto itr = begin<T>();
    itr += static_cast<typename const_iterator<T>::difference_type>(this->size_in_bytes() / sizeof(T));
    return itr;
}

//------------------------------------------------------------------------------

template<typename T>
auto array::range()
{
    auto b = begin<T>();
    auto e = end<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
auto array::range() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

//------------------------------------------------------------------------------

template<typename T>
auto array::crange() const
{
    auto b = cbegin<T>();
    auto e = cend<T>();
    return boost::make_iterator_range(b, e);
}

} // namespace sight::data
