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

#include <data/helper/medical_image.hpp>
#include <data/point_list.hpp>
#include <data/series.hpp>
#include <data/string.hpp>
#include <data/types.hpp>
#include <data/vector.hpp>

#include <boost/algorithm/string/split.hpp>

#include <sstream>

namespace sight::io::dicom::writer::tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::DicomInstance)& _instance,
    const data::image::csptr& _image
) :
    io::dicom::writer::tid::TemplateID<data::image>(_writer, _instance, _image)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
= default;

//------------------------------------------------------------------------------

void Measurement::createNodes(
    const SPTR(io::dicom::container::sr::DicomSRNode)& _parent,
    bool _use_s_coord3_d
)
{
    data::vector::sptr distance_vector = data::helper::medical_image::get_distances(*m_object);
    if(distance_vector)
    {
        unsigned int id = 1;
        for(const data::object::sptr& object : *distance_vector)
        {
            data::point_list::sptr point_list = std::dynamic_pointer_cast<data::point_list>(object);
            if(point_list)
            {
                this->createMeasurement(_parent, point_list, id++, _use_s_coord3_d);
            }
        }
    }
}

//------------------------------------------------------------------------------

void Measurement::createMeasurement(
    const SPTR(io::dicom::container::sr::DicomSRNode)& _parent,
    const data::point_list::csptr& _point_list,
    unsigned int /*id*/,
    bool _use_s_coord3_d
)
{
    const data::point::sptr point1 = _point_list->getPoints()[0];
    const data::point::sptr point2 = _point_list->getPoints()[1];

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
    const std::size_t frame_number1 = io::dicom::helper::DicomDataTools::convertPointToFrameNumber(m_object, point1);

    // Create Measurement Node
    // cspell: ignore UCUM
    SPTR(io::dicom::container::sr::DicomSRNumNode) num_node =
        std::make_shared<io::dicom::container::sr::DicomSRNumNode>(
            io::dicom::container::DicomCodedAttribute("121206", "DCM", "Distance"),
            "CONTAINS",
            distance,
            io::dicom::container::DicomCodedAttribute("mm", "UCUM", "millimeter", "1.4")
        );
    _parent->addSubNode(num_node);

    if(_use_s_coord3_d)
    {
        // Create SCoord Node
        std::vector<float> scoord_vector {
            static_cast<float>(point1->getCoord()[0]),
            static_cast<float>(point1->getCoord()[1]),
            static_cast<float>(point1->getCoord()[2]),
            static_cast<float>(point2->getCoord()[0]),
            static_cast<float>(point2->getCoord()[1]),
            static_cast<float>(point2->getCoord()[2])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoord_node =
            std::make_shared<io::dicom::container::sr::DicomSRSCoord3DNode>(
                io::dicom::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM",
                "POLYLINE",
                scoord_vector,
                m_instance->getSOPInstanceUIDContainer()[0]
            );
        num_node->addSubNode(scoord_node);
    }
    else
    {
        SIGHT_ASSERT(
            "Unable to save a 3D distance using a SCOORD object.",
            frame_number1 == io::dicom::helper::DicomDataTools::convertPointToFrameNumber(m_object, point2)
        );

        // Create SCoord Node
        std::vector<float> scoord_vector {
            static_cast<float>(point1->getCoord()[0]),
            static_cast<float>(point1->getCoord()[1]),
            static_cast<float>(point2->getCoord()[0]),
            static_cast<float>(point2->getCoord()[1])
        };
        SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoord_node =
            std::make_shared<io::dicom::container::sr::DicomSRSCoordNode>(
                io::dicom::container::DicomCodedAttribute("121230", "DCM", "Path"),
                "INFERRED FROM",
                "POLYLINE",
                scoord_vector
            );
        num_node->addSubNode(scoord_node);

        // Create Image Node
        SPTR(io::dicom::container::sr::DicomSRImageNode) image_node =
            std::make_shared<io::dicom::container::sr::DicomSRImageNode>(
                io::dicom::container::DicomCodedAttribute(),
                "SELECTED FROM",
                m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frame_number1 - 1],
                int(frame_number1)
            );
        scoord_node->addSubNode(image_node);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
