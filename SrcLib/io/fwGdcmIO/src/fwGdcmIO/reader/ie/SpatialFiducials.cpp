/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/helper/DicomDataTools.hpp"
#include "fwGdcmIO/reader/ie/SpatialFiducials.hpp"

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwMedData/DicomSeries.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

SpatialFiducials::SpatialFiducials(const SPTR(::fwMedData::DicomSeries)& dicomSeries,
                                   const SPTR(::gdcm::Reader)& reader,
                                   const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                   const ::fwData::Image::sptr& image,
                                   const ::fwLog::Logger::sptr& logger,
                                   ProgressCallback progress,
                                   CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwData::Image >(dicomSeries, reader, instance, image,
                                                                 logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducials::~SpatialFiducials()
{
}

//------------------------------------------------------------------------------

void SpatialFiducials::readLandmark(const ::gdcm::DataSet& fiducialDataset)
{
    ::fwData::PointList::sptr pointList =
            m_object->getField< ::fwData::PointList >
(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
    if(!pointList)
    {
        pointList = ::fwData::PointList::New();
        m_object->setField(::fwDataTools::fieldHelper::Image::m_imageLandmarksId, pointList);
    }

    const ::gdcm::DataElement& graphicCoordinatesDataElement =
            fiducialDataset.GetDataElement( ::gdcm::Tag(0x0070, 0x0318) );
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > graphicCoordinatesDataSequence =
            graphicCoordinatesDataElement.GetValueAsSQ();

    const std::string label =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0070, 0x030F >(fiducialDataset);

    for(unsigned int i = 1; i <= graphicCoordinatesDataSequence->GetNumberOfItems(); ++i)
    {
        ::gdcm::Item graphicCoordinatesItem = graphicCoordinatesDataSequence->GetItem(i);
        const ::gdcm::DataSet& graphicCoordinatesDataset = graphicCoordinatesItem.GetNestedDataSet();

        // 2D Points
        ::gdcm::Attribute< 0x0070, 0x0022 > coordinatesAttribute;
        coordinatesAttribute.SetFromDataElement(graphicCoordinatesDataset.GetDataElement(::gdcm::Tag(0x0070, 0x0022)));
        const float* pointValues = coordinatesAttribute.GetValues();

        // Frame number
        const ::gdcm::DataElement& referencedImageDataElement =
                graphicCoordinatesDataset.GetDataElement( ::gdcm::Tag(0x0008, 0x1140) );
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > referencedImageDataSequence =
                referencedImageDataElement.GetValueAsSQ();
        ::gdcm::Item referencedImageItem = referencedImageDataSequence->GetItem(1);
        ::gdcm::DataSet& referencedImageDataset = referencedImageItem.GetNestedDataSet();

        int frameNumber =
::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x1160, int >(referencedImageDataset);
        double zCoordinate =
::fwGdcmIO::helper::DicomDataTools::convertFrameNumberToZCoordinate(m_object,frameNumber);

        ::fwData::Point::sptr point = ::fwData::Point::New(static_cast<double>(pointValues[0]),
                                                           static_cast<double>(pointValues[1]), zCoordinate);
        point->setField(::fwDataTools::fieldHelper::Image::m_labelId, ::fwData::String::New(label));
        pointList->getRefPoints().push_back(point);

    }
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
