/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "data/timeline/Buffer.hpp"

namespace sight::data
{

namespace timeline
{

/**
 * @brief   This class is the non-template part of the class GenericObject. It is used to store groups of objects
 *          inside a timeline. The maximum number of elements inside an object is fixed, however some elements
 *          can be missing. A mask is provided to test the presence of a given element.
 *          Note that the size of an element is not determined by dividing the size of the buffer by the maximum
 *          number of elements. Thus it is possible to have objects bigger
 */
class DATA_CLASS_API GenericObjectBase : public data::timeline::Buffer
{
public:

    /// Destructor
    DATA_API virtual ~GenericObjectBase();

    /// Return the number of elements present in the object
    DATA_API unsigned int getPresentElementNum() const;

    /// Tell if an element is present at the given index
    DATA_API bool isPresent(unsigned int index) const;

    /// Return the raw presence mask
    DATA_API uint64_t getMask() const;

    /// Return the maximum number of elements in the buffer
    DATA_API unsigned int getMaxElementNum() const;

    /// Return the size of element in the buffer
    DATA_API size_t getElementSize() const;

    /// Make a copy of this buffer
    DATA_API virtual void deepCopy(const data::timeline::Object& other);

protected:

    /// Constructor
    DATA_API GenericObjectBase(
        unsigned int maxElementNum,
        core::HiResClock::HiResClockType timestamp = 0,
        BufferDataType buffer                      = 0,
        size_t size                                = 0,
        DeleterType d                              = 0
    );

    /// Number of elements that are actually set
    unsigned int m_numPresent;
    /// Binary mask that indicates which element are set
    uint64_t m_presenceMask;
    /// Maximum number of elements in an object
    unsigned int m_maxElementNum;
};

/**
 * @brief   This timeline is used to store a group of objects of a given type. The maximum number of elements inside an
 *          object is fixed, however some elements can be missing. A mask is provided to test the presence of a given
 *          element.
 */
template<typename TYPE>
class GenericObject : public GenericObjectBase
{
public:

    /// Type of the elements inside the buffer
    typedef TYPE ElementType;

    class iterator
    {
    public:

        /// Go to the next element
        void operator++();

        /// True if the current element is valid.
        bool isValid() const
        {
            return m_currentIndex < m_maxElement;
        }

        /// Get the current element
        const ElementType& operator*() const;

    private:

        /// Constructor
        iterator(const GenericObjectBase& object);

        /// Pointer on the buffer object
        const GenericObjectBase* m_object;

        /// Current element
        unsigned int m_currentIndex;

        /// Maximum number of elements in the buffer
        unsigned int m_maxElement;

        friend class GenericObject<ElementType>;
    };

    friend class iterator;

    /// Constructor
    GenericObject(
        unsigned int m_maxElementNum,
        core::HiResClock::HiResClockType timestamp = 0,
        BufferDataType buffer                      = 0,
        size_t size                                = 0,
        DeleterType d                              = 0
    );

    /// Destructor
    virtual ~GenericObject();

    /// Return the nth element in the buffer
    const TYPE& getElement(unsigned int index) const;

    /// Set the nth element in the buffer. Element in parameter will be copied at the given index.
    void setElement(const ElementType& element, unsigned int index);

    /// Add an element and return a pointer on the newly added element
    TYPE* addElement(unsigned int index);

    /// Return an iterator on the elements present in the object
    iterator getPresenceIterator() const;
};

} // namespace timeline

} // namespace sight::data
