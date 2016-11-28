/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedDataCamp/AttachmentSeries.hpp"

#include <fwCamp/UserObject.hpp>

#include <fwMemory/BufferObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(AttachmentSeries))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("attachment_availability", &::fwMedData::AttachmentSeries::m_attachmentAvailability)
    .property("attachment_path", &::fwMedData::AttachmentSeries::m_attachmentPath)
    .property("media_type", &::fwMedData::AttachmentSeries::m_mediaType)
    .property("buffer", &::fwMedData::AttachmentSeries::m_buffer)
    ;
}


fwCampImplementEnumMacro((fwMedData)(AttachmentSeries)(AttachmentAvailability))
{
    builder
    .value("NONE",  ::fwMedData::AttachmentSeries::NONE)
    .value("LOCAL",  ::fwMedData::AttachmentSeries::LOCAL)
    .value("EMBEDDED",  ::fwMedData::AttachmentSeries::EMBEDDED)
    ;
}
