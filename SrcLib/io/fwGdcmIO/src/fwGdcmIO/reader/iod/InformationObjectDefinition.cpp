/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

InformationObjectDefinition::InformationObjectDefinition(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                                                         const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                                         const ::fwLog::Logger::sptr& logger,
                                                         ProgressCallback progress,
                                                         CancelRequestedCallback cancel) :
    m_instance(instance),
    m_dicomSeries(dicomSeries),
    m_logger(logger),
    m_progressCallback(progress),
    m_cancelRequestedCallback(cancel)
{
    SLM_ASSERT("DicomSeries should not be null.", dicomSeries);
    SLM_ASSERT("Instance should not be null.", instance);
    SLM_ASSERT("Logger should not be null.", logger);
}

//------------------------------------------------------------------------------

InformationObjectDefinition::~InformationObjectDefinition()
{
}

//------------------------------------------------------------------------------

} // namespace iod
} // namespace reader
} // namespace fwGdcmIO
