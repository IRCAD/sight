/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/reader/ie/SpatialFiducials.hpp"

#include "io/dicom/helper/DicomDataReader.hxx"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/DicomSeries.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/Image.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

namespace sight::io::dicom
{

namespace reader
{

namespace ie
{

//------------------------------------------------------------------------------

SpatialFiducials::SpatialFiducials(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(::gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Image::sptr& image,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::ie::InformationEntity<data::Image>(dicomSeries, reader, instance, image,
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
    data::PointList::sptr pointList =
        m_object->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);
    if(!pointList)
    {
        pointList = data::PointList::New();
        m_object->setField(data::fieldHelper::Image::m_imageLandmarksId, pointList);
    }

    const ::gdcm::DataElement& graphicCoordinatesDataElement =
        fiducialDataset.GetDataElement(::gdcm::Tag(0x0070, 0x0318));
    const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> graphicCoordinatesDataSequence =
        graphicCoordinatesDataElement.GetValueAsSQ();

    const std::string label =
        io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x030F>(fiducialDataset);

    for(unsigned int i = 1 ; i <= graphicCoordinatesDataSequence->GetNumberOfItems() ; ++i)
    {
        ::gdcm::Item graphicCoordinatesItem              = graphicCoordinatesDataSequence->GetItem(i);
        const ::gdcm::DataSet& graphicCoordinatesDataset = graphicCoordinatesItem.GetNestedDataSet();

        // 2D Points
        ::gdcm::Attribute<0x0070, 0x0022> coordinatesAttribute;
        coordinatesAttribute.SetFromDataElement(graphicCoordinatesDataset.GetDataElement(::gdcm::Tag(0x0070, 0x0022)));
        const float* pointValues = coordinatesAttribute.GetValues();

        // Frame number
        const ::gdcm::DataElement& referencedImageDataElement =
            graphicCoordinatesDataset.GetDataElement(::gdcm::Tag(0x0008, 0x1140));
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems> referencedImageDataSequence =
            referencedImageDataElement.GetValueAsSQ();
        ::gdcm::Item referencedImageItem        = referencedImageDataSequence->GetItem(1);
        ::gdcm::DataSet& referencedImageDataset = referencedImageItem.GetNestedDataSet();

        int frameNumber =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1160, int>(referencedImageDataset);
        double zCoordinate =
            io::dicom::helper::DicomDataTools::convertFrameNumberToZCoordinate(m_object, frameNumber);

        data::Point::sptr point = data::Point::New(
            static_cast<double>(pointValues[0]),
            static_cast<double>(pointValues[1]),
            zCoordinate
        );
        point->setField(data::fieldHelper::Image::m_labelId, data::String::New(label));
        pointList->getPoints().push_back(point);
    }
}

//------------------------------------------------------------------------------

} // namespace ie

} // namespace reader

} // namespace sight::io::dicom
