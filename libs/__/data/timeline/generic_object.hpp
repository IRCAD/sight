/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
#include "data/timeline/buffer.hpp"

namespace sight::data::timeline
{

/**
 * @brief   This class is the non-template part of the class generic_object. It is used to store groups of objects
 *          inside a timeline. The maximum number of elements inside an object is fixed, however some elements
 *          can be missing. A mask is provided to test the presence of a given element.
 *          Note that the size of an element is not determined by dividing the size of the buffer by the maximum
 *          number of elements. Thus it is possible to have objects bigger
 */
class DATA_CLASS_API generic_object_base : public data::timeline::buffer
{
public:

    /// Destructor
    DATA_API ~generic_object_base() override;

    /// Return the number of elements present in the object
    [[nodiscard]] DATA_API unsigned int getPresentElementNum() const;

    /// Tell if an element is present at the given index
    [[nodiscard]] DATA_API bool isPresent(unsigned int _index) const;

    /// Return the raw presence mask
    [[nodiscard]] DATA_API uint64_t getMask() const;

    /// Return the maximum number of elements in the buffer
    [[nodiscard]] DATA_API unsigned int getMaxElementNum() const;

    /// Return the size of element in the buffer
    [[nodiscard]] DATA_API std::size_t getElementSize() const;

    /// Make a copy of this buffer
    DATA_API void deep_copy(const data::timeline::object& _other) override;

protected:

    /// Constructor
    DATA_API generic_object_base(
        unsigned int _max_element_num,
        core::hires_clock::type _timestamp = 0,
        buffer_data_t _buffer              = nullptr,
        std::size_t _size                  = 0,
        deleter_t _d                       = nullptr
    );

    /// Number of elements that are actually set
    unsigned int m_numPresent {0};
    /// Binary mask that indicates which element are set
    uint64_t m_presenceMask {0};
    /// Maximum number of elements in an object
    unsigned int m_maxElementNum;
};

/**
 * @brief   This timeline is used to store a group of objects of a given type. The maximum number of elements inside an
 *          object is fixed, however some elements can be missing. A mask is provided to test the presence of a given
 *          element.
 */
template<typename TYPE>
class generic_object : public generic_object_base
{
public:

    /// Type of the elements inside the buffer
    typedef TYPE element_t;

    class iterator
    {
    public:

        /// Go to the next element
        void operator++();

        /// True if the current element is valid.
        [[nodiscard]] bool isValid() const
        {
            return m_currentIndex < m_maxElement;
        }

        /// Get the current element
        const element_t& operator*() const;

    private:

        /// Constructor
        iterator(const generic_object_base& _object);

        /// Pointer on the buffer object
        const generic_object_base* m_object;

        /// Current element
        unsigned int m_currentIndex {0};

        /// Maximum number of elements in the buffer
        unsigned int m_maxElement;

        friend class generic_object<element_t>;
    };

    friend class iterator;

    /// Constructor
    generic_object(
        unsigned int _m_max_element_num,
        core::hires_clock::type _timestamp = 0,
        buffer_data_t _buffer              = nullptr,
        std::size_t _size                  = 0,
        deleter_t _d                       = nullptr
    );

    /// Destructor
    ~generic_object() override;

    /// Return the nth element in the buffer
    [[nodiscard]] const TYPE& getElement(unsigned int _index) const;

    /// Set the nth element in the buffer. Element in parameter will be copied at the given index. The method is
    /// disabled if TYPE isn't TriviallyCopyable because setElement internally uses memcpy.
    void setElement(const element_t& _element, unsigned int _index);

    /// Add an element and return a pointer on the newly added element
    TYPE* addElement(unsigned int _index);

    /// Return an iterator on the elements present in the object
    [[nodiscard]] iterator getPresenceIterator() const;
};

} // namespace sight::data::timeline
