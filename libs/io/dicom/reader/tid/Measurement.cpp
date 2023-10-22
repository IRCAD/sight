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

#include "io/dicom/reader/tid/Measurement.hpp"

#include "io/dicom/container/sr/DicomSRImageNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoord3DNode.hpp"
#include "io/dicom/container/sr/DicomSRSCoordNode.hpp"
#include "io/dicom/container/sr/DicomSRTextNode.hpp"
#include "io/dicom/helper/DicomDataTools.hpp"

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

namespace sight::io::dicom::reader::tid
{

//------------------------------------------------------------------------------

Measurement::Measurement(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::DicomInstance::sptr& _instance,
    const data::image::sptr& _image,
    const core::log::logger::sptr& _logger
) :
    io::dicom::reader::tid::TemplateID<data::image>(_dicom_series, _reader, _instance, _image, _logger)
{
}

//------------------------------------------------------------------------------

Measurement::~Measurement()
= default;

//------------------------------------------------------------------------------

void Measurement::readNode(const SPTR(io::dicom::container::sr::DicomSRNode)& _node)
{
    if(_node->getCodedAttribute() == io::dicom::container::DicomCodedAttribute("121206", "DCM", "Distance")
       && !_node->getSubNodeContainer().empty())
    {
        for(const SPTR(io::dicom::container::sr::DicomSRNode) & sub_node : _node->getSubNodeContainer())
        {
            if(sub_node->getType() == "SCOORD")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoordNode) scoord_node =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoordNode>(sub_node);
                if(scoord_node && scoord_node->get_graphic_type() == "POLYLINE")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::graphic_data_container_t coordinates =
                        scoord_node->getGraphicDataContainer();

                    if(!scoord_node->getSubNodeContainer().empty())
                    {
                        SPTR(io::dicom::container::sr::DicomSRImageNode) image_node =
                            std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRImageNode>(
                                *scoord_node->getSubNodeContainer().begin()
                            );
                        if(image_node)
                        {
                            const int frame_number = image_node->getFrameNumber();
                            double z_coordinate    = io::dicom::helper::DicomDataTools::convertFrameNumberToZCoordinate(
                                m_object,
                                std::size_t(frame_number)
                            );

                            auto origin = std::make_shared<data::point>(
                                static_cast<double>(coordinates[0]),
                                static_cast<double>(coordinates[1]),
                                z_coordinate
                            );
                            auto destination = std::make_shared<data::point>(
                                static_cast<double>(coordinates[2]),
                                static_cast<double>(coordinates[3]),
                                z_coordinate
                            );
                            this->addDistance(origin, destination);
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(sub_node->getType() == "SCOORD3D")
            {
                SPTR(io::dicom::container::sr::DicomSRSCoord3DNode) scoord3_d_node =
                    std::dynamic_pointer_cast<io::dicom::container::sr::DicomSRSCoord3DNode>(sub_node);
                if(scoord3_d_node && scoord3_d_node->get_graphic_type() == "POLYLINE")
                {
                    // Retrieve coordinates
                    io::dicom::container::sr::DicomSRSCoordNode::graphic_data_container_t coordinates =
                        scoord3_d_node->getGraphicDataContainer();
                    this->addDistance(
                        std::make_shared<data::point>(coordinates[0], coordinates[1], coordinates[2]),
                        std::make_shared<data::point>(coordinates[3], coordinates[4], coordinates[5])
                    );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void Measurement::addDistance(
    const SPTR(data::point)& _point1,
    const SPTR(data::point)& _point2
)
{
    auto distance_vector = data::helper::medical_image::get_distances(*m_object);

    if(!distance_vector)
    {
        distance_vector = std::make_shared<data::vector>();
        data::helper::medical_image::set_distances(*m_object, distance_vector);
    }

    data::point_list::sptr point_list = std::make_shared<data::point_list>();
    point_list->getPoints().push_back(_point1);
    point_list->getPoints().push_back(_point2);

    distance_vector->push_back(point_list);
    data::helper::medical_image::set_distance_visibility(*m_object, true);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::tid
