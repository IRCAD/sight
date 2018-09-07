/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedData/AttachmentSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

#include <fwMemory/stream/in/Raw.hpp>

#include <boost/filesystem/operations.hpp>

fwDataRegisterMacro( ::fwMedData::AttachmentSeries );

namespace fwMedData
{

AttachmentSeries::AttachmentSeries(::fwData::Object::Key key) :
    Series(key),
    m_buffer(::fwMemory::BufferObject::New()),
    m_attachmentAvailability(NONE)
{
    FW_DEPRECATED_MSG("'AttachmentSeries' is deprecated.", "20.0");
}

//------------------------------------------------------------------------------

AttachmentSeries::~AttachmentSeries()
{
}

//------------------------------------------------------------------------------

void AttachmentSeries::cachedDeepCopy(const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache)
{
    AttachmentSeries::csptr other = AttachmentSeries::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );

    this->::fwMedData::Series::cachedDeepCopy(_source, cache);

    if( !other->m_buffer->isEmpty() )
    {
        ::fwMemory::BufferObject::Lock lockerDest(m_buffer);
        m_buffer->allocate(other->m_buffer->getSize());
        char* buffDest = static_cast< char* >( lockerDest.getBuffer() );

        ::fwMemory::BufferObject::Lock lockerSource(other->m_buffer);
        char* buffSrc = static_cast< char* >( lockerSource.getBuffer() );

        std::copy(buffSrc, buffSrc+other->m_buffer->getSize(), buffDest );
    }
    m_attachmentAvailability = other->m_attachmentAvailability;
    m_attachmentPath         = other->m_attachmentPath;
    m_mediaType              = other->m_mediaType;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path& AttachmentSeries::getAttachmentPath() const
{
    return m_attachmentPath;
}

//------------------------------------------------------------------------------

void AttachmentSeries::setAttachmentPath(const ::boost::filesystem::path& path)
{
    m_attachmentPath = path;
    size_t buffSize = ::boost::filesystem::file_size(path);
    m_buffer->setIStreamFactory( std::make_shared< ::fwMemory::stream::in::Raw >(path),
                                 buffSize, path, ::fwMemory::RAW);
    m_attachmentAvailability = LOCAL;
}

//------------------------------------------------------------------------------

::fwMemory::BufferObject::sptr AttachmentSeries::getBufferObject () const
{
    return m_buffer;
}

//------------------------------------------------------------------------------

AttachmentSeries::AttachmentAvailability AttachmentSeries::getAttachmentAvailability() const
{
    return m_attachmentAvailability;
}

//------------------------------------------------------------------------------

std::string AttachmentSeries::getMediaType() const
{
    return m_mediaType;
}

//------------------------------------------------------------------------------

void AttachmentSeries::setMediaType(const std::string& mediaType)
{
    m_mediaType = mediaType;
}

//------------------------------------------------------------------------------

} // namespace fwMedData
