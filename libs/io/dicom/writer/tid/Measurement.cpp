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

#include "io/dicom/writer/tid/Measurement.hpp"

#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRCodeNode.hpp"
#include "io/dicom/container/sr/DicomSRImageNode.hpp"
#include "io/dicom/container/sr/DicomSRNumNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoord3DNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/container/sr/DicomSRUIDRefNode.hpp"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/PointList.hpp>
#include <data/Series.hpp>
#include <data/String.hpp>
#include <data/types.hpp>
#include <data/Vector.hpp>

#include <boost/algorithm/string/split.hpp>

#include <sstream>

namespace sight::io::dicom::writer::tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(
    const SPTR(gdcm::Writer)& writer,
    const SPTR(io::dicom::container::DicomInstance)& instance,
    const data::Image::csptr& image
) :
    io::dicom::writer::tid::TemplateID<data::Image>(writer, instance, image)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
= default;

//------------------------------------------------------------------------------

void Measurement::createNodes(
    const SPTR(io::dicom::container::sr::DicomSRNode)& parent,
    bool useSCoord3D
)
{
    data::Vector::sptr distanceVector = data::helper::MedicalImage::getDistances(*m_object);
    if(distanceVector)
    {
        unsigned int id = 1;
        for(const data::Object::sptr& object : *distanceVector)
        {
            data::PointList::sptr pointList = std::dynamic_pointer_cast<data::PointList>(object);
            if(pointList)
            {
                this->createMeasurement(parent, pointList, id++, useSCoord3D);
            }
        }
    }
}

//------------------------------------------------------------------------------

void Measurement::createMeasurement(
    const SPTR(io::dicom::container::sr::DicomSRNode)& parent,
    const data::PointList::csptr& pointList,
    unsigned int /*id*/,
    bool useSCoord3D
)
{
    const data::Point::sptr point1 = pointList->getPoints()[0];
    const data::Point::sptr point2 = pointList->getPoints()[1];

    std::array coordinates {
        point1->getCoord()[0],
        point1->getCoord()[1],
        point1->getCoord()[2],
        point2->getCoord()[0],
        point2->getCoord()[1],
        point2->getCoord()[2]
    };

    const double distance = sqrt(
        (coordinates[0] - coordinates[3]) * (coordinates[0] - coordinates[3])
        + (coordinates[1] - coordinates[4]) * (coordinates[1] - coordinates[4])
        + (coordinates[2] - coordinates[5]) * (coordinates[2] - coordinates[5])
    );

    // Retrieve Frame Numbers
    const std::size_t frameNumber1 = io::dicom::helper::DicomDataTools::convertPointToFrameNumber(m_object, point1);

    // Create Measurement Node
    // cspell: ignore UCUM
    SPTR(io::dicom::container::sr::DicomSRNumNode) numNode =
        std::make_shared<io::dicom::container::sr::DicomSRNumNode>(
            io::dicom::container::DicomCodedAttribute("121206", "DCM", "Distance"),
            "CONTAINS",
            distance,
            io::dicom::container::DicomCodedAttribute("mm", "UCUM", "millimeter", "1.4")
        );
    parent->addSubNode(numNode);

    if(useSCoord3D)
    {
        // Create SCoord Node
        std::vector<float> scoordVector {
            static_cast<float>(point1->getCoord()[0]),
            static_cast<float>(point1->getCoord()[1]),
            static_cast<float>(point1->getCoord()[2]),
            static_cast<float>(point2->getCoord()[0]),
            static_cast<float>(point2->getCoord()[1]),
            static_cast<float>(point2->getCoord()[2])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoordNode =
            std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                io::dicom::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM",
                "POLYLINE",
                scoordVector,
                m_instance->getSOPInstanceUIDContainer()[0]
            );
        numNode->addSubNode(scoordNode);
    }
    else
    {
        SIGHT_ASSERT(
            "Unable to save a 3D distance using a SCOORD object.",
            frameNumber1 == io::dicom::helper::DicomDataTools::convertPointToFrameNumber(m_object, point2)
        );

        // Create SCoord Node
        std::vector<float> scoordVector {
            static_cast<float>(point1->getCoord()[0]),
            static_cast<float>(point1->getCoord()[1]),
            static_cast<float>(point2->getCoord()[0]),
            static_cast<float>(point2->getCoord()[1])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoordNode =
            std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                io::dicom::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM",
                "POLYLINE",
                scoordVector
            );
        numNode->addSubNode(scoordNode);

        // Create Image Node
        SPTR(io::dicom::container::sr::DicomSRImageNode) imageNode =
            std::make_shared<io::dicom::container::sr::DicomSRImageNode>(
                io::dicom::container::DicomCodedAttribute(),
                "SELECTED FROM",
                m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frameNumber1 - 1],
                int(frameNumber1)
            );
        scoordNode->addSubNode(imageNode);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
