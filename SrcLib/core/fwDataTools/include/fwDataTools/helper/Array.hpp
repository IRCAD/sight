/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

namespace fwDataTools
{

namespace helper
{

/**
 * @brief   Helper to manage array buffer. Lock the buffer before to modify it.
 */
class FWDATATOOLS_CLASS_API Array
{

public:
    fwCoreClassFactoryMacro((Array), (((::fwData::Array::sptr))), new Array )

    FWDATATOOLS_API Array( ::fwData::Array::sptr array );
    FWDATATOOLS_API virtual ~Array();

    /**
     * @brief Setter for one item components of the array
     *
     * @param id Item id
     * @param value Valid buffer of elements of type \<m_type\> with a length equal to \<m_nbOfComponents\> to be copied
     * to array 'id'
     */
    FWDATATOOLS_API virtual void setItem(const ::fwData::Array::IndexType& id, const void* value);

    /**
     * @brief Setter for one item component of the array
     *
     * @param id Item id
     * @param component Component id to write data in.
     * @param value Valid buffer of elements of type \<m_type\> with a length equal to \<m_nbOfComponents\> to be copied
     * to array 'id', component index 'component'
     *
     */
    FWDATATOOLS_API virtual void setItem(const ::fwData::Array::IndexType& id, const size_t component,
                                         const void* value);

    ///
    /**
     * @brief Getter for a buffer item. pointer to the requested item in the buffer
     *
     * @param id Item id
     * @param component Component id
     *
     * @return Pointer to the requested item in the buffer
     */
    FWDATATOOLS_API virtual void* getItem(const ::fwData::Array::IndexType& id, const size_t component = 0);

    /**
     * @brief Typed version of getItem
     *
     * @tparam T Type in which the pointer will be returned
     * @param id Item id
     * @param component Component id
     *
     * @return Array buffer pointer casted to T
     */
    template< typename T > T* getItem(const ::fwData::Array::IndexType& id, const size_t component = 0);

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
     * @{
     */
    FWDATATOOLS_API virtual void* getBuffer();
    FWDATATOOLS_API virtual const void* getBuffer() const;
    ///@}

    /**
     * @brief Setter for the array buffer.
     *
     * An existing buffer will be released if the array own it.
     *
     * @param buf            Buffer to set as Array's buffer
     * @param takeOwnership  if true, the Array will manage allocation and destroy the buffer when needed.
     * @param type           Type of the array view
     * @param size           Size of the array view
     * @param nbOfComponents Number of components of the array view, Min value : 1
     * @param policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    FWDATATOOLS_API void setBuffer(
        void* buf,
        bool takeOwnership,
        const ::fwTools::Type& type,
        const ::fwData::Array::SizeType& size,
        size_t nbOfComponents,
        ::fwMemory::BufferAllocationPolicy::sptr policy = ::fwMemory::BufferMallocPolicy::New()
        );

    /// Returns the begining/end of the buffer interpreted as a char buffer
    FWDATATOOLS_API virtual char* begin();
    FWDATATOOLS_API virtual char* end();
    FWDATATOOLS_API virtual const char* begin() const;
    FWDATATOOLS_API virtual const char* end() const;

    /// Returns the begining/end of the buffer, casted to T
    template< typename T > T* begin();
    template< typename T > T* end();

    /**
     * @brief Get a pointer to the value described by given parameters
     *
     * @param id Item id
     * @param component Item component id
     * @param sizeOfType size of a component
     *
     * @return buffer item pointer
     * @{
     */
    FWDATATOOLS_API char* getBufferPtr( const ::fwData::Array::IndexType& id, size_t component, size_t sizeOfType );
    FWDATATOOLS_API const char* getBufferPtr( const ::fwData::Array::IndexType& id, size_t component,
                                              size_t sizeOfType ) const;
    ///@}

    /// Returns a copy of current lock on array
    FWDATATOOLS_API ::fwMemory::BufferObject::Lock getLock() const;

protected:

    /**
     * @brief Protected setter for the array buffer.
     * An existing buffer will be released if the array own it.
     *
     * @param buf Buffer to set as Array's buffer
     * @param takeOwnership if true, the Array will manage allocation and destroy the buffer when needed.
     * @param policy If the array takes ownership of the buffer, specifies the buffer allocation policy.
     */
    FWDATATOOLS_API virtual void setBuffer(
        void* buf,
        bool takeOwnership                              = false,
        ::fwMemory::BufferAllocationPolicy::sptr policy = ::fwMemory::BufferMallocPolicy::New()
        );

    ::fwData::Array::sptr m_array;
    ::fwMemory::BufferObject::Lock m_lock;

};

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
    return reinterpret_cast<T*> (static_cast<char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//------------------------------------------------------------------------------

template< typename T >
T* Array::getItem(const ::fwData::Array::IndexType& id, const size_t component)
{
    return static_cast<T*> (this->getItem(id, component));
}

} // namespace helper

} // namespace fwData
