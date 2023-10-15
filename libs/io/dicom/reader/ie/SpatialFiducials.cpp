/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/dicom_series.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

SpatialFiducials::SpatialFiducials(
    const data::dicom_series::csptr& dicomSeries,
    const SPTR(gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::image::sptr& image,
    const core::log::logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::ie::InformationEntity<data::image>(dicomSeries, reader, instance, image,
                                                          logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducials::~SpatialFiducials()
= default;

//------------------------------------------------------------------------------

void SpatialFiducials::readLandmark(const gdcm::DataSet& fiducialDataset)
{
    data::point_list::sptr pointList = data::helper::MedicalImage::getLandmarks(*m_object);

    if(!pointList)
    {
        pointList = std::make_shared<data::point_list>();
        data::helper::MedicalImage::setLandmarks(*m_object, pointList);
    }

    const gdcm::DataElement& graphicCoordinatesDataElement =
        fiducialDataset.GetDataElement(gdcm::Tag(0x0070, 0x0318));
    const gdcm::SmartPointer<gdcm::SequenceOfItems> graphicCoordinatesDataSequence =
        graphicCoordinatesDataElement.GetValueAsSQ();

    const std::string label =
        io::dicom::helper::DicomDataReader::getTagValue<0x0070, 0x030F>(fiducialDataset);

    for(unsigned int i = 1 ; i <= graphicCoordinatesDataSequence->GetNumberOfItems() ; ++i)
    {
        gdcm::Item graphicCoordinatesItem              = graphicCoordinatesDataSequence->GetItem(i);
        const gdcm::DataSet& graphicCoordinatesDataset = graphicCoordinatesItem.GetNestedDataSet();

        // 2D Points
        gdcm::Attribute<0x0070, 0x0022> coordinatesAttribute;
        coordinatesAttribute.SetFromDataElement(graphicCoordinatesDataset.GetDataElement(gdcm::Tag(0x0070, 0x0022)));
        const float* pointValues = coordinatesAttribute.GetValues();

        // Frame number
        const gdcm::DataElement& referencedImageDataElement =
            graphicCoordinatesDataset.GetDataElement(gdcm::Tag(0x0008, 0x1140));
        const gdcm::SmartPointer<gdcm::SequenceOfItems> referencedImageDataSequence =
            referencedImageDataElement.GetValueAsSQ();
        gdcm::Item referencedImageItem        = referencedImageDataSequence->GetItem(1);
        gdcm::DataSet& referencedImageDataset = referencedImageItem.GetNestedDataSet();

        int frameNumber =
            io::dicom::helper::DicomDataReader::getTagValue<0x0008, 0x1160, int>(referencedImageDataset);
        double zCoordinate =
            io::dicom::helper::DicomDataTools::convertFrameNumberToZCoordinate(m_object, std::size_t(frameNumber));

        data::point::sptr point = std::make_shared<data::point>(
            static_cast<double>(pointValues[0]),
            static_cast<double>(pointValues[1]),
            zCoordinate
        );
        point->setLabel(label);
        pointList->getPoints().push_back(point);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
