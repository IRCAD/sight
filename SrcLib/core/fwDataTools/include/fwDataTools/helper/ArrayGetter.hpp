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

#include "fwDataTools/config.hpp"

#include <fwData/Array.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/Type.hpp>

namespace fwDataTools
{

namespace helper
{

/**
 * @brief   Helper to manage array buffer. Lock the buffer before to modify it.
 *
 */
class FWDATATOOLS_CLASS_API ArrayGetter
{

public:

    /// Shared pointer type
    typedef std::shared_ptr < ArrayGetter > sptr;
    /// Const shared pointer type
    typedef std::shared_ptr < const ArrayGetter > csptr;
    /// Unique pointer type
    typedef std::unique_ptr < ArrayGetter > uptr;
    /// Const unique pointer type
    typedef std::unique_ptr < const ArrayGetter > cuptr;

    //------------------------------------------------------------------------------

    static sptr New(::fwData::Array::csptr array)
    {
        return std::make_shared<ArrayGetter>(array);
    }

    FWDATATOOLS_API ArrayGetter(const ::fwData::Array::csptr& array );

    FWDATATOOLS_API virtual ~ArrayGetter();

    ///
    /**
     * @brief Getter for a buffer item. pointer to the requested item in the buffer
     *
     * @param id Item id
     * @param component Component id
     *
     * @return Pointer to the requested item in the buffer
     */
    FWDATATOOLS_API virtual const void* getItem(const ::fwData::Array::IndexType& id, const size_t component = 0) const;

    /**
     * @brief Typed version of getItem
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     * @param component Component id
     *
     * @return Array buffer pointer casted to T
     */
    template< typename T > const T* getItem(const ::fwData::Array::IndexType& id, const size_t component = 0) const;

    /**
     * @brief Copies the data into the buffer pointed by <value>
     *
     * @param id Item id
     * @param[out] value Buffer to write into
     */
    FWDATATOOLS_API virtual void getItem(const ::fwData::Array::IndexType& id, void* value) const;
    FWDATATOOLS_API virtual void getItem(const ::fwData::Array::IndexType& id, const size_t component,
                                         void* value) const;

    /**
     * @brief Getter for the array buffer
     *
     * @return Array's buffer, if exists, else NULL
     */
    FWDATATOOLS_API virtual const void* getBuffer() const;

    /// Returns the begining/end of the buffer interpreted as a char buffer
    FWDATATOOLS_API virtual const char* begin() const;
    FWDATATOOLS_API virtual const char* end() const;

    /// Returns the begining/end of the buffer, casted to T
    template< typename T > const T* begin() const;
    template< typename T > const T* end() const;

    /**
     * @brief Get a pointer to the value described by given parameters
     *
     * @param id Item id
     * @param component Item component id
     * @param sizeOfType size of a component
     *
     * @return buffer item pointer
     */
    FWDATATOOLS_API const char* getBufferPtr( const ::fwData::Array::IndexType& id, size_t component,
                                              size_t sizeOfType ) const;

    /// Returns a copy of current lock on array
    FWDATATOOLS_API ::fwMemory::BufferObject::Lock getLock() const;

protected:

    ::fwData::Array::csptr m_array;
    ::fwMemory::BufferObject::Lock m_lock;

};

//------------------------------------------------------------------------------

template< typename T >
const T* ArrayGetter::begin() const
{
    return static_cast<const T*>(this->getBuffer());
}

//------------------------------------------------------------------------------

template< typename T >
const T* ArrayGetter::end() const
{
    return reinterpret_cast<const T*> (static_cast<const char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//------------------------------------------------------------------------------

template< typename T >
const T* ArrayGetter::getItem(const ::fwData::Array::IndexType& id, const size_t component) const
{
    return static_cast<const T*> (this->getItem(id, component));
}

} // namespace helper

} // namespace fwData
