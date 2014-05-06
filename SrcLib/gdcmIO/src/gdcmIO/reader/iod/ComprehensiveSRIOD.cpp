/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwMedData/ImageSeries.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Document.hpp"

#include "gdcmIO/reader/iod/ComprehensiveSRIOD.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

ComprehensiveSRIOD::ComprehensiveSRIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcmIO::container::DicomInstance) instance) :
        ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
}

//------------------------------------------------------------------------------

ComprehensiveSRIOD::~ComprehensiveSRIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ComprehensiveSRIOD::read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Create GDCM reader
    SPTR(::gdcm::Reader) reader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );

    // Read the first file
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Unable to read the DICOM instance using the GDCM Image Reader."),
            !success);

    // Create Information Entity helpers
    ::gdcmIO::reader::ie::Document documentIE(m_dicomSeries, reader, m_instance, imageSeries->getImage());

    // Read SR
    documentIE.readSR();

}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

