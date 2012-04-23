/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_HELPER_ARRAY_HPP_
#define _FWCOMED_HELPER_ARRAY_HPP_

#include <fwTools/Type.hpp>
#include <fwTools/BufferObject.hpp>

#include <fwData/Array.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{

namespace helper
{

/**
 * @class   Array
 * @brief   ...
 * @author  IRCAD (Research and Development Team).
 * @date    2012
 */
class FWCOMED_CLASS_API Array
{
public :

    FWCOMED_API Array( ::fwData::Array::sptr array );

    FWCOMED_API virtual ~Array();

    /**
     * @brief Getter for the array buffer
     *
     * @return Array's buffer, if exists, else NULL
     * @{
     */
    FWCOMED_API virtual void *getBuffer();
    FWCOMED_API virtual const void *getBuffer() const;
    ///@}


    /// Returns the begining/end of the buffer interpreted as a char buffer
    FWCOMED_API virtual char* begin();
    FWCOMED_API virtual char* end();
    FWCOMED_API virtual const char* begin() const;
    FWCOMED_API virtual const char* end() const;


    /// Returns the begining/end of the buffer, casted to T
    template< typename T > T* begin();
    template< typename T > T* end();

protected:

    ::fwData::Array::sptr m_array;
    ::fwTools::BufferObject::Lock m_lock;

};

template< typename T >
T* Array::begin()
{
    return static_cast<T*>(this->getBuffer());
}

template< typename T >
T* Array::end()
{
    return reinterpret_cast<T*> (static_cast<char*>(this->getBuffer()) + m_array->getSizeInBytes());
}



} // namespace helper

} // namespace fwData

#endif // _FWCOMED_HELPER_ARRAY_HPP_
