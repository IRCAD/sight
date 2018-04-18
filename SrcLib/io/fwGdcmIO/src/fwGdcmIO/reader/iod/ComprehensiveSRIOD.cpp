/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/iod/ComprehensiveSRIOD.hpp"

#include "fwGdcmIO/reader/ie/Document.hpp"

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwMedData/ImageSeries.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

ComprehensiveSRIOD::ComprehensiveSRIOD(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                                       const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                       const ::fwLog::Logger::sptr& logger,
                                       ProgressCallback progress,
                                       CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

ComprehensiveSRIOD::~ComprehensiveSRIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ComprehensiveSRIOD::read(::fwMedData::Series::sptr series) throw(::fwGdcmIO::exception::Failed)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(::gdcm::Reader) reader = std::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );

    // Read the first file
    ::fwMedData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Unable to read the DICOM instance \""+
                                                        filename+"\" using the GDCM Image Reader."), !success);

    // Create Information Entity helpers
    ::fwGdcmIO::reader::ie::Document documentIE(m_dicomSeries, reader, m_instance, imageSeries->getImage(), m_logger,
                                                m_progressCallback, m_cancelRequestedCallback);

    // Read SR
    documentIE.readSR();

}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
