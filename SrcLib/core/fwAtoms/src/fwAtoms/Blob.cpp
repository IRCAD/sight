/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/Blob.hpp"

fwAtomsRegisterMacro( ::fwAtoms::Blob );

namespace fwAtoms
{

//------------------------------------------------------------------------------

Blob::sptr Blob::New(::fwMemory::BufferObject::sptr buffer)
{
    Blob::sptr blob = Blob::New();
    blob->m_bufferObject = buffer;

    return blob;
}

//------------------------------------------------------------------------------

Blob::sptr Blob::New(void* buffer, unsigned int size)
{
    ::fwMemory::BufferObject::sptr bufferObject = ::fwMemory::BufferObject::New();
    bufferObject->setBuffer(buffer, size);
    Blob::sptr blob = Blob::New(bufferObject);
    return blob;
}

//------------------------------------------------------------------------------

::fwMemory::BufferObject::sptr Blob::getBufferObject() const
{
    return m_bufferObject;
}

//------------------------------------------------------------------------------

void Blob::setBufferObject(const ::fwMemory::BufferObject::sptr &bo)
{
    m_bufferObject = bo;
}

Base::sptr Blob::clone() const
{
    ::fwMemory::BufferObject::sptr bufferObjectDest = ::fwMemory::BufferObject::New();
    Blob::sptr cloneBlob = Blob::New(bufferObjectDest);
    if(m_bufferObject)
    {
        bufferObjectDest->allocate(m_bufferObject->getSize());

        ::fwMemory::BufferObject::Lock lockerSource(m_bufferObject);
        char * buffSrc = static_cast< char * >( lockerSource.getBuffer() );

        ::fwMemory::BufferObject::Lock lockerDest(bufferObjectDest);
        char * buffDest = static_cast< char * >( lockerDest.getBuffer() );

        std::copy(buffSrc, buffSrc + m_bufferObject->getSize(), buffDest );
    }
    return cloneBlob;
}

}

