/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_TIMELINE_GENERICOBJECT_HPP__
#define __EXTDATA_TIMELINE_GENERICOBJECT_HPP__

#include "extData/config.hpp"
#include "extData/timeline/Buffer.hpp"

namespace extData
{

namespace timeline
{

/**
 * @class   GenericObjectBase
 * @brief   This class is the non-template part of the class GenericObject. It is used to store groups of objects
 *          inside a timeline. The maximum number of elements inside an object is fixed, however some elements
 *          can be missing. A mask is provided to test the presence of a given element.
 *          Note that the size of an element is not determined by dividing the size of the buffer by the maximum
 *          number of elements. Thus it is possible to have objects bigger
 */
class EXTDATA_CLASS_API GenericObjectBase : public ::extData::timeline::Buffer
{
public:

    /// Destructor
    EXTDATA_API virtual ~GenericObjectBase();

    /// Return the number of elements present in the object
    EXTDATA_API unsigned int getPresentElementNum() const;

    /// Tell if an element is present at the given index
    EXTDATA_API bool isPresent(unsigned int index) const;

    /// Return the raw presence mask
    EXTDATA_API uint64_t getMask() const;

    /// Return the maximum number of elements in the buffer
    EXTDATA_API unsigned int getMaxElementNum() const;

    /// Return the size of element in the buffer
    EXTDATA_API size_t getElementSize() const;

    /// Make a copy of this buffer
    EXTDATA_API virtual void deepCopy(const ::extData::timeline::Object& other);

protected:

    /// Constructor
    EXTDATA_API GenericObjectBase( unsigned int maxElementNum, ::fwCore::HiResClock::HiResClockType timestamp = 0,
                                   BufferDataType buffer = 0, size_t size = 0, DeleterType d = 0);

    /// Number of elements that are actually set
    unsigned int m_numPresent;
    /// Binary mask that indicates which element are set
    uint64_t m_presenceMask;
    /// Maximum number of elements in an object
    unsigned int m_maxElementNum;
};

/**
 * @class   GenericObject
 * @brief   This timeline is used to store a group of objects of a given type. The maximum number of elements inside an
 *          object is fixed, however some elements can be missing. A mask is provided to test the presence of a given
 *          element.
 */
template< typename TYPE >
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
    GenericObject( unsigned int m_maxElementNum, ::fwCore::HiResClock::HiResClockType timestamp = 0,
                   BufferDataType buffer = 0, size_t size = 0, DeleterType d = 0);

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

} // namespace extData

#endif /* __EXTDATA_TIMELINE_GENERICOBJECT_HPP__ */
