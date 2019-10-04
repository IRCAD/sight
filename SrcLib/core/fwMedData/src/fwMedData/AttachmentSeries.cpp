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

#include "fwMedData/AttachmentSeries.hpp"

#include <fwData/Exception.hpp>
#include <fwData/registry/macros.hpp>

#include <fwMemory/stream/in/Raw.hpp>

#include <filesystem>

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

const std::filesystem::path& AttachmentSeries::getAttachmentPath() const
{
    return m_attachmentPath;
}

//------------------------------------------------------------------------------

void AttachmentSeries::setAttachmentPath(const std::filesystem::path& path)
{
    m_attachmentPath = path;
    size_t buffSize = std::filesystem::file_size(path);
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
