/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/iod/SpatialFiducialsIOD.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/reader/ie/SpatialFiducials.hpp"

#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <boost/algorithm/string/trim.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

SpatialFiducialsIOD::SpatialFiducialsIOD(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                                         const ::fwGdcmIO::container::DicomInstance::sptr& instance,
                                         const ::fwLog::Logger::sptr& logger,
                                         ProgressCallback progress,
                                         CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducialsIOD::~SpatialFiducialsIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::read(::fwMedData::Series::sptr series) throw (::fwGdcmIO::exception::Failed)
{
    // Retrieve images
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("ImageSeries should not be null.", imageSeries);
    ::fwData::Image::sptr image = imageSeries->getImage();
    SLM_ASSERT("::fwData::Image not instanced", image);

    // Create GDCM Reader
    SPTR(::gdcm::Reader) reader = std::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );

    // Read the first file
    ::fwMedData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();

    if(pathContainer.size() > 1)
    {
        m_logger->warning("More than one Spatial Fiducials files have been found in the series. "
                          "Only the first one will be read.");
    }

    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Unable to read the DICOM instance \""+
                                                        filename+"\" using the GDCM Image Reader."), !success);

    // Create Information Entity helpers
    ::fwGdcmIO::reader::ie::SpatialFiducials spatialFiducialsIE(
        m_dicomSeries, reader, m_instance, imageSeries->getImage(),
        m_logger, m_progressCallback, m_cancelRequestedCallback);

    // Retrieve dataset
    const ::gdcm::DataSet& datasetRoot = reader->GetFile().GetDataSet();

    // Retrieve Fiducial Set Sequence
    const ::gdcm::DataElement& fiducialSetSequenceDataElement =
        datasetRoot.GetDataElement( ::gdcm::Tag(0x0070, 0x031C) );
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > fiducialSetSequence =
        fiducialSetSequenceDataElement.GetValueAsSQ();

    for(unsigned int i = 1; i <= fiducialSetSequence->GetNumberOfItems(); ++i)
    {
        ::gdcm::Item sequenceSetItem = fiducialSetSequence->GetItem(i);
        const ::gdcm::DataSet& sequenceSetDataset = sequenceSetItem.GetNestedDataSet();

        const ::gdcm::DataElement& fiducialSequenceDataElement =
            sequenceSetDataset.GetDataElement( ::gdcm::Tag(0x0070, 0x031E) );
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > fiducialSequence =
            fiducialSequenceDataElement.GetValueAsSQ();

        for(unsigned int j = 1; j <= fiducialSequence->GetNumberOfItems(); ++j)
        {
            ::gdcm::Item fiducialItem = fiducialSequence->GetItem(j);
            const ::gdcm::DataSet& fiducialDataset = fiducialItem.GetNestedDataSet();
            const std::string shapeType            =
                ::fwGdcmIO::helper::DicomDataReader::getTagValue<0x0070, 0x0306>(fiducialDataset);

            if(shapeType == "POINT")
            {
                spatialFiducialsIE.readLandmark(fiducialDataset);
            }
            else
            {
                m_logger->warning("Fiducial shape type not supported: \"" + shapeType + "\"");
            }
        }

    }
}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
