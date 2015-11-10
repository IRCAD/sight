/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/trim.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>
#include <fwMedData/ImageSeries.hpp>

#include "gdcmIO/reader/iod/SpatialFiducialsIOD.hpp"
#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/SpatialFiducials.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

SpatialFiducialsIOD::SpatialFiducialsIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
                                         SPTR(::gdcmIO::container::DicomInstance)instance) :
    ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
}

//------------------------------------------------------------------------------

SpatialFiducialsIOD::~SpatialFiducialsIOD()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::read(::fwMedData::Series::sptr series) throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    // Retrieve images
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("ImageSeries should not be null.", imageSeries);
    ::fwData::Image::sptr image = imageSeries->getImage();
    SLM_ASSERT("::fwData::Image not instanced", image);

    // Create GDCM Reader
    SPTR(::gdcm::Reader) reader = std::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );

    // Read the first file
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    OSLM_WARN_IF("More than one Spatial Fiducials file was found in series. Only the first one will be read.",
                 pathContainer.size() >1);
    const std::string filename = pathContainer.begin()->second.string();
    reader->SetFileName( filename.c_str() );
    bool success = reader->Read();
    FW_RAISE_EXCEPTION_IF(::gdcmIO::exception::Failed("Unable to read the DICOM instance using the GDCM Image Reader."),
                          !success);

    // Create Information Entity helpers
    ::gdcmIO::reader::ie::SpatialFiducials spatialFiducialsIE(
        m_dicomSeries, reader, m_instance, imageSeries->getImage());

    // Retrieve dataset
    const ::gdcm::DataSet &datasetRoot = reader->GetFile().GetDataSet();

    // Retrieve Fiducial Set Sequence
    const ::gdcm::DataElement& fiducialSetSequenceDataElement =
        datasetRoot.GetDataElement( ::gdcm::Tag(0x0070, 0x031C) );
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > fiducialSetSequence =
        fiducialSetSequenceDataElement.GetValueAsSQ();

    for(unsigned int i = 1; i <= fiducialSetSequence->GetNumberOfItems(); ++i)
    {
        ::gdcm::Item sequenceSetItem        = fiducialSetSequence->GetItem(i);
        ::gdcm::DataSet& sequenceSetDataset = sequenceSetItem.GetNestedDataSet();

        const ::gdcm::DataElement& fiducialSequenceDataElement =
            sequenceSetDataset.GetDataElement( ::gdcm::Tag(0x0070, 0x031E) );
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > fiducialSequence =
            fiducialSequenceDataElement.GetValueAsSQ();

        for(unsigned int j = 1; j <= fiducialSequence->GetNumberOfItems(); ++j)
        {
            ::gdcm::Item fiducialItem        = fiducialSequence->GetItem(j);
            ::gdcm::DataSet& fiducialDataset = fiducialItem.GetNestedDataSet();
            const std::string shapeType =
                ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0070,0x0306>(fiducialDataset);

            if(shapeType == "POINT")
            {
                spatialFiducialsIE.readLandmark(fiducialDataset);
            }
            else
            {
                OSLM_WARN("Fiducial shape type not supported: \"" << shapeType << "\"");
            }
        }

    }
}

//------------------------------------------------------------------------------

}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

