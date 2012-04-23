/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwComEd/helper/Array.hpp"

namespace fwComEd
{

namespace helper
{

Array::Array( ::fwData::Array::sptr array ) :
        m_array (array)
{
    OSLM_ASSERT("Array ptr is null.", array);
    m_lock = array->getBufferObject()->lock();
}

//-----------------------------------------------------------------------------

Array::~Array()
{

}

//-----------------------------------------------------------------------------

void *Array::getBuffer()
{
    return m_lock.getBuffer();
}

//-----------------------------------------------------------------------------

const void *Array::getBuffer() const
{
    return m_lock.getBuffer();
}

//-----------------------------------------------------------------------------

char *Array::begin()
{
    void *v = this->getBuffer();
    return static_cast<char*>(v);
}

//------------------------------------------------------------------------------

char *Array::end()
{
    return reinterpret_cast<char*> (static_cast<char*>(this->getBuffer()) + m_array->getSizeInBytes());
}
//------------------------------------------------------------------------------

const char *Array::begin() const
{
    return static_cast<const char*>(this->getBuffer());
}

//------------------------------------------------------------------------------

const char *Array::end() const
{
    return reinterpret_cast<const char*> (static_cast<const char*>(this->getBuffer()) + m_array->getSizeInBytes());
}

//-----------------------------------------------------------------------------


} // namespace helper

} // namespace fwComEd
