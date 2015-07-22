/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include "gdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

InformationObjectDefinition::InformationObjectDefinition(::fwDicomData::DicomSeries::sptr dicomSeries,
                                                         SPTR(::gdcmIO::container::DicomInstance)instance) :
    m_dicomSeries(dicomSeries), m_instance(instance)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);
    SLM_ASSERT("Instance should not be null.", instance);
}

//------------------------------------------------------------------------------

InformationObjectDefinition::~InformationObjectDefinition()
{
}

} // namespace iod
} // namespace reader
} // namespace gdcmIO
