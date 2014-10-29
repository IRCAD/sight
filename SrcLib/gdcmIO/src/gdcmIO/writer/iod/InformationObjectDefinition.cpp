/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include "gdcmIO/writer/iod/InformationObjectDefinition.hpp"

namespace gdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

InformationObjectDefinition::InformationObjectDefinition(
        SPTR(::gdcmIO::container::DicomInstance) instance, ::boost::filesystem::path folderPath):
        m_instance(instance), m_folderPath(folderPath)
{
    SLM_ASSERT("Instance should not be null.", instance);
}

//------------------------------------------------------------------------------

InformationObjectDefinition::~InformationObjectDefinition()
{
}

} // namespace iod
} // namespace writer
} // namespace gdcmIO
