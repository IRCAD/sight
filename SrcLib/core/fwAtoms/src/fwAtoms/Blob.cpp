/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/Blob.hpp"


fwCampImplementMacro((fwAtoms)(Blob))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwAtoms")
        .base< ::fwAtoms::Base >();
}

fwAtomsRegisterMacro( ::fwAtoms::Blob );

namespace fwAtoms
{

//------------------------------------------------------------------------------

Blob::sptr Blob::New(::fwTools::BufferObject::sptr buffer)
{
    Blob::sptr blob = Blob::New();
    blob->m_bufferObject = buffer;

    return blob;
}

//------------------------------------------------------------------------------

Blob::sptr Blob::New(void* buffer, unsigned int size)
{
    ::fwTools::BufferObject::sptr bufferObject = ::fwTools::BufferObject::New();
    bufferObject->setBuffer(buffer, size);
    Blob::sptr blob = Blob::New(bufferObject);
    return blob;
}

//------------------------------------------------------------------------------

size_t Blob::getSize() const
{
    return m_bufferObject->getSize();
}

//------------------------------------------------------------------------------

char* Blob::getBuffer() const
{
    const ::fwTools::BufferObject::Lock& lock = m_bufferObject->lock();
    return static_cast<char *>( lock.getBuffer());
}

//------------------------------------------------------------------------------

::fwTools::BufferObject::sptr Blob::getBufferObject() const
{
    return m_bufferObject;
}

//------------------------------------------------------------------------------

Base::sptr Blob::clone() const
{
    ::fwTools::BufferObject::sptr bufferObjectDest = ::fwTools::BufferObject::New();
    Blob::sptr cloneBlob = Blob::New(bufferObjectDest);
    if(m_bufferObject)
    {
        bufferObjectDest->allocate(m_bufferObject->getSize());

        ::fwTools::BufferObject::Lock lockerSource(m_bufferObject);
        char * buffSrc = static_cast< char * >( lockerSource.getBuffer() );

        ::fwTools::BufferObject::Lock lockerDest(bufferObjectDest);
        char * buffDest = static_cast< char * >( lockerDest.getBuffer() );

        std::copy(buffSrc, buffSrc + m_bufferObject->getSize(), buffDest );
    }
    return cloneBlob;
}

}
