/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwAtoms/Blob.hpp"

#include "fwAtoms/registry/macros.hpp"

fwAtomsRegisterMacro( ::fwAtoms::Blob );

namespace fwAtoms
{

//------------------------------------------------------------------------------

Blob::sptr Blob::New(core::memory::BufferObject::sptr buffer)
{
    Blob::sptr blob = Blob::New();
    blob->m_bufferObject = buffer;

    return blob;
}

//------------------------------------------------------------------------------

Blob::sptr Blob::New(void* buffer, unsigned int size)
{
    core::memory::BufferObject::sptr bufferObject = core::memory::BufferObject::New();
    bufferObject->setBuffer(buffer, size);
    Blob::sptr blob = Blob::New(bufferObject);
    return blob;
}

//------------------------------------------------------------------------------

core::memory::BufferObject::sptr Blob::getBufferObject() const
{
    return m_bufferObject;
}

//------------------------------------------------------------------------------

void Blob::setBufferObject(const core::memory::BufferObject::sptr& bo)
{
    m_bufferObject = bo;
}

//------------------------------------------------------------------------------

Base::sptr Blob::clone() const
{
    core::memory::BufferObject::sptr bufferObjectDest = core::memory::BufferObject::New();
    Blob::sptr cloneBlob                              = Blob::New(bufferObjectDest);
    if(m_bufferObject)
    {
        bufferObjectDest->allocate(m_bufferObject->getSize());

        core::memory::BufferObject::Lock lockerSource(m_bufferObject);
        char* buffSrc = static_cast< char* >( lockerSource.getBuffer() );

        core::memory::BufferObject::Lock lockerDest(bufferObjectDest);
        char* buffDest = static_cast< char* >( lockerDest.getBuffer() );

        std::copy(buffSrc, buffSrc + m_bufferObject->getSize(), buffDest );
    }
    return cloneBlob;
}

}
