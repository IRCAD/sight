/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/dicom/reader/tid/Measurement.hpp"

#include "io/dicom/container/sr/DicomSRImageNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoord3DNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/Boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

namespace sight::io::dicom
{

namespace reader
{

namespace tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Image::sptr& image,
    const core::log::Logger::sptr& logger
) :
    io::dicom::reader::tid::TemplateID<data::Image>(dicomSeries, reader, instance, image, logger)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
{
}

//------------------------------------------------------------------------------

void Measurement::readNode(const SPTR(io::dicom::container::sr::DicomSRNode)& node)
{
    if(node->getCodedAttribute() == io::dicom::container::DicomCodedAttribute("121206", "DCM", "Distance")
       && !node->getSubNodeContainer().empty())
    {
        for(const SPTR(io::dicom::container::sr::DicomSRNode) & subNode : node->getSubNodeContainer())
        {
            if(subNode->getType() == "SCOORD")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoordNode =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoordNode>(subNode);
                if(scoordNode && scoordNode->getGraphicType() == "POLYLINE")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoordNode->getGraphicDataContainer();

                    if(!scoordNode->getSubNodeContainer().empty())
                    {
                        SPTR(io::dicom::container::sr::DicomSRImageNode) imageNode =
                            std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRImageNode>(
                                *scoordNode->getSubNodeContainer().begin()
                            );
                        if(imageNode)
                        {
                            const int frameNumber = imageNode->getFrameNumber();
                            double zCoordinate    = io::dicom::helper::DicomDataTools::convertFrameNumberToZCoordinate(
                                m_object,
                                frameNumber
                            );

                            auto origin = data::Point::New(
                                static_cast<double>(coordinates[0]),
                                static_cast<double>(coordinates[1]),
                                zCoordinate
                            );
                            auto destination = data::Point::New(
                                static_cast<double>(coordinates[2]),
                                static_cast<double>(coordinates[3]),
                                zCoordinate
                            );
                            this->addDistance(origin, destination);
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(subNode->getType() == "SCOORD3D")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoord3DNode =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoord3DNode>(subNode);
                if(scoord3DNode && scoord3DNode->getGraphicType() == "POLYLINE")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoord3DNode->getGraphicDataContainer();
                    this->addDistance(
                        data::Point::New(coordinates[0], coordinates[1], coordinates[2]),
                        data::Point::New(coordinates[3], coordinates[4], coordinates[5])
                    );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void Measurement::addDistance(
    const SPTR(data::Point)& point1,
    const SPTR(data::Point)& point2
)
{
    auto distanceVector = data::helper::MedicalImage::getDistances(*m_object);

    if(!distanceVector)
    {
        distanceVector = data::Vector::New();
        data::helper::MedicalImage::setDistances(*m_object, distanceVector);
    }

    data::PointList::sptr pointList = data::PointList::New();
    pointList->getPoints().push_back(point1);
    pointList->getPoints().push_back(point2);

    distanceVector->push_back(pointList);
    data::helper::MedicalImage::setDistanceVisibility(*m_object, true);
}

//------------------------------------------------------------------------------

} // namespace tid

} // namespace reader

} // namespace sight::io::dicom
