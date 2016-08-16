/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/writer/iod/InformationObjectDefinition.hpp"

#include <fwCore/spyLog.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

InformationObjectDefinition::InformationObjectDefinition(
    SPTR(::fwGdcmIO::container::DicomInstance)instance, ::boost::filesystem::path folderPath) :
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
} // namespace fwGdcmIO
