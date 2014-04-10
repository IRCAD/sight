/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
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
#include "gdcmIO/helper/DicomSR.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

SpatialFiducialsIOD::SpatialFiducialsIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcmIO::container::DicomInstance) instance) :
        ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
    // Instantiates a specific reader
    m_gdcmReader = ::boost::shared_ptr< ::gdcm::Reader >( new ::gdcm::Reader );
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

    // Path container
    ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
    OSLM_WARN_IF("More than one Spatial Fiducials file was found in series. Only the first one will be read.",
            pathContainer.size() >1);

    // Read first file
    std::string filename = pathContainer.begin()->second.string();
    m_gdcmReader->SetFileName( filename.c_str() );
    if ( m_gdcmReader->Read() )
    {
        // Retrieve dataset
        const ::gdcm::DataSet &datasetRoot = m_gdcmReader->GetFile().GetDataSet();

        // Retireve Fiducial Set Sequence
        const ::gdcm::DataElement& fiducialSetSequenceDataElement =
                datasetRoot.GetDataElement( ::gdcm::Tag(0x0070, 0x031C) );
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > fiducialSetSequence =
                fiducialSetSequenceDataElement.GetValueAsSQ();

        for(unsigned int i = 1; i <= fiducialSetSequence->GetNumberOfItems(); ++i)
        {
            ::gdcm::Item sequenceSetItem = fiducialSetSequence->GetItem(i);
            ::gdcm::DataSet& sequenceSetDataset = sequenceSetItem.GetNestedDataSet();

            const ::gdcm::DataElement& fiducialSequenceDataElement =
                    sequenceSetDataset.GetDataElement( ::gdcm::Tag(0x0070, 0x031E) );
            const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > fiducialSequence =
                    fiducialSequenceDataElement.GetValueAsSQ();

            for(unsigned int j = 1; j <= fiducialSequence->GetNumberOfItems(); ++j)
            {
                ::gdcm::Item fiducialItem = fiducialSequence->GetItem(j);
                ::gdcm::DataSet& fiducialDataset = fiducialItem.GetNestedDataSet();
                const std::string shapeType =
                        ::gdcmIO::helper::DicomData::getTrimmedTagValue<0x0070,0x0306>(fiducialDataset);

                if(shapeType == "POINT")
                {
                    this->readLandmark(image, fiducialDataset);
                }
                else if(shapeType == "LINE")
                {
                    this->readDistance(image, fiducialDataset);
                }
                else
                {
                    OSLM_WARN("Fiducial shape type not supported: \"" << shapeType << "\"");
                }
            }

        }
    }

}

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::readLandmark(::fwData::Image::sptr image, ::gdcm::DataSet& fiducialDataset)
{
    ::fwData::PointList::sptr pointList =
            image->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);
    if(!pointList)
    {
        pointList = ::fwData::PointList::New();
        image->setField(::fwComEd::Dictionary::m_imageLandmarksId, pointList);
    }

    const ::gdcm::DataElement& graphicCoordinatesDataElement =
            fiducialDataset.GetDataElement( ::gdcm::Tag(0x0070, 0x0318) );
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > graphicCoordinatesDataSequence =
            graphicCoordinatesDataElement.GetValueAsSQ();

    const std::string label = ::gdcmIO::helper::DicomData::getTagValue< 0x0070, 0x030F >(fiducialDataset);

    for(unsigned int i = 1; i <= graphicCoordinatesDataSequence->GetNumberOfItems(); ++i)
    {
        ::gdcm::Item graphicCoordinatesItem = graphicCoordinatesDataSequence->GetItem(i);
        ::gdcm::DataSet& graphicCoordinatesDataset= graphicCoordinatesItem.GetNestedDataSet();

        ::gdcm::Attribute< 0x0070, 0x0022 > coordinatesAttribute;
        coordinatesAttribute.SetFromDataElement(graphicCoordinatesDataset.GetDataElement(::gdcm::Tag(0x0070, 0x0022)));
        const float* pointValues = coordinatesAttribute.GetValues();

        ::fwData::Point::sptr point = ::fwData::Point::New(pointValues[0], pointValues[1], pointValues[2]);
        point->setField(::fwComEd::Dictionary::m_labelId, ::fwData::String::New(label));
        pointList->getRefPoints().push_back(point);

    }
}

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::readDistance(::fwData::Image::sptr image, ::gdcm::DataSet& fiducialDataset)
{
    ::fwData::Vector::sptr distanceVector = image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
    if(!distanceVector)
    {
        distanceVector = ::fwData::Vector::New();
        image->setField(::fwComEd::Dictionary::m_imageDistancesId, distanceVector);
    }

    const ::gdcm::DataElement& graphicCoordinatesDataElement =
            fiducialDataset.GetDataElement( ::gdcm::Tag(0x0070, 0x0318) );
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > graphicCoordinatesDataSequence =
            graphicCoordinatesDataElement.GetValueAsSQ();

    for(unsigned int i = 1; i <= graphicCoordinatesDataSequence->GetNumberOfItems(); ++i)
    {
        ::gdcm::Item graphicCoordinatesItem = graphicCoordinatesDataSequence->GetItem(i);
        ::gdcm::DataSet& graphicCoordinatesDataset= graphicCoordinatesItem.GetNestedDataSet();

        ::gdcm::Attribute< 0x0070, 0x0022 > coordinatesAttribute;
        coordinatesAttribute.SetFromDataElement(graphicCoordinatesDataset.GetDataElement(::gdcm::Tag(0x0070, 0x0022)));
        const float* pointValues = coordinatesAttribute.GetValues();

        ::fwData::Point::sptr point1 = ::fwData::Point::New(pointValues[0], pointValues[1], pointValues[2]);
        ::fwData::Point::sptr point2 = ::fwData::Point::New(pointValues[3], pointValues[4], pointValues[5]);

        ::fwData::PointList::sptr pointList = ::fwData::PointList::New();
        pointList->getRefPoints().push_back(point1);
        pointList->getRefPoints().push_back(point2);
        distanceVector->getContainer().push_back(pointList);

    }
}


}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO

