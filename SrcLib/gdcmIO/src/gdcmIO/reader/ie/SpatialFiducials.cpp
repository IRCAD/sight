/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <gdcmUIDGenerator.h>

#include <fwComEd/Dictionary.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>
#include <fwDicomData/DicomSeries.hpp>

#include <fwTools/dateAndTime.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/SpatialFiducials.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

SpatialFiducials::SpatialFiducials(SPTR(::fwDicomData::DicomSeries) dicomSeries,
        SPTR(::gdcm::Reader) reader,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwData::Image::sptr image):
        ::gdcmIO::reader::ie::InformationEntity< ::fwData::Image >(dicomSeries, reader, instance, image)
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
            m_object->getField< ::fwData::PointList >(::fwComEd::Dictionary::m_imageLandmarksId);
    if(!pointList)
    {
        pointList = ::fwData::PointList::New();
        m_object->setField(::fwComEd::Dictionary::m_imageLandmarksId, pointList);
    }

    const ::gdcm::DataElement& graphicCoordinatesDataElement =
            fiducialDataset.GetDataElement( ::gdcm::Tag(0x0070, 0x0318) );
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > graphicCoordinatesDataSequence =
            graphicCoordinatesDataElement.GetValueAsSQ();

    const std::string label = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0070, 0x030F >(fiducialDataset);



    for(unsigned int i = 1; i <= graphicCoordinatesDataSequence->GetNumberOfItems(); ++i)
    {
        ::gdcm::Item graphicCoordinatesItem = graphicCoordinatesDataSequence->GetItem(i);
        ::gdcm::DataSet& graphicCoordinatesDataset= graphicCoordinatesItem.GetNestedDataSet();

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
        ::gdcm::DataSet& referencedImageDataset= referencedImageItem.GetNestedDataSet();

        int frameNumber = ::gdcmIO::helper::DicomData::getTagValue< 0x0008, 0x1160, int >(referencedImageDataset);
        float zCoordinate = ::gdcmIO::helper::DicomData::convertFrameNumberToZCoordinate(m_object,frameNumber);

        ::fwData::Point::sptr point = ::fwData::Point::New(pointValues[0], pointValues[1], zCoordinate);
        point->setField(::fwComEd::Dictionary::m_labelId, ::fwData::String::New(label));
        pointList->getRefPoints().push_back(point);

    }
}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace gdcmIO
