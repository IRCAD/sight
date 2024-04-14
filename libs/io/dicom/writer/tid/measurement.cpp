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

#include "io/dicom/writer/tid/measurement.hpp"

#include "io/dicom/container/dicom_coded_attribute.hpp"
#include "io/dicom/container/sr/dicom_sr_code_node.hpp"
#include "io/dicom/container/sr/dicom_sr_image_node.hpp"
#include "io/dicom/container/sr/dicom_sr_num_node.hpp"
#include "io/dicom/container/sr/dicom_sr_text_node.hpp"
#include "io/dicom/container/sr/dicom_srs_coord3dnode.hpp"
#include "io/dicom/container/sr/dicom_srs_coord_node.hpp"
#include "io/dicom/container/sr/dicom_sruid_ref_node.hpp"
#include "io/dicom/helper/dicom_data_tools.hpp"

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

measurement::measurement(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::image::csptr& _image
) :
    io::dicom::writer::tid::template_id<data::image>(_writer, _instance, _image)
{
}

//------------------------------------------------------------------------------

measurement::~measurement()
= default;

//------------------------------------------------------------------------------

void measurement::create_nodes(
    const SPTR(io::dicom::container::sr::dicom_sr_node)& _parent,
    bool _use_s_coord_3d
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
                this->create_measurement(_parent, point_list, id++, _use_s_coord_3d);
            }
        }
    }
}

//------------------------------------------------------------------------------

void measurement::create_measurement(
    const SPTR(io::dicom::container::sr::dicom_sr_node)& _parent,
    const data::point_list::csptr& _point_list,
    unsigned int /*id*/,
    bool _use_s_coord_3d
)
{
    const data::point::sptr point1 = _point_list->get_points()[0];
    const data::point::sptr point2 = _point_list->get_points()[1];

    std::array coordinates {
        point1->get_coord()[0],
        point1->get_coord()[1],
        point1->get_coord()[2],
        point2->get_coord()[0],
        point2->get_coord()[1],
        point2->get_coord()[2]
    };

    const double distance = sqrt(
        (coordinates[0] - coordinates[3]) * (coordinates[0] - coordinates[3])
        + (coordinates[1] - coordinates[4]) * (coordinates[1] - coordinates[4])
        + (coordinates[2] - coordinates[5]) * (coordinates[2] - coordinates[5])
    );

    // Retrieve Frame Numbers
    const std::size_t frame_number1 = io::dicom::helper::dicom_data_tools::convert_point_to_frame_number(
        m_object,
        point1
    );

    // Create Measurement Node
    // cspell: ignore UCUM
    SPTR(io::dicom::container::sr::dicom_sr_num_node) num_node =
        std::make_shared<io::dicom::container::sr::dicom_sr_num_node>(
            io::dicom::container::dicom_coded_attribute("121206", "DCM", "Distance"),
            "CONTAINS",
            distance,
            io::dicom::container::dicom_coded_attribute("mm", "UCUM", "millimeter", "1.4")
        );
    _parent->add_sub_node(num_node);

    if(_use_s_coord_3d)
    {
        // Create SCoord Node
        std::vector<float> scoord_vector {
            static_cast<float>(point1->get_coord()[0]),
            static_cast<float>(point1->get_coord()[1]),
            static_cast<float>(point1->get_coord()[2]),
            static_cast<float>(point2->get_coord()[0]),
            static_cast<float>(point2->get_coord()[1]),
            static_cast<float>(point2->get_coord()[2])
        };
        SPTR(io::dicom::container::sr::dicom_srs_coord3_d_node) scoord_node =
            std::make_shared<io::dicom::container::sr::dicom_srs_coord3_d_node>(
                io::dicom::container::dicom_coded_attribute("121230", "DCM", "Path"),
                "INFERRED FROM",
                "POLYLINE",
                scoord_vector,
                m_instance->get_sop_instance_uid_container()[0]
            );
        num_node->add_sub_node(scoord_node);
    }
    else
    {
        SIGHT_ASSERT(
            "Unable to save a 3D distance using a SCOORD object.",
            frame_number1 == io::dicom::helper::dicom_data_tools::convert_point_to_frame_number(m_object, point2)
        );

        // Create SCoord Node
        std::vector<float> scoord_vector {
            static_cast<float>(point1->get_coord()[0]),
            static_cast<float>(point1->get_coord()[1]),
            static_cast<float>(point2->get_coord()[0]),
            static_cast<float>(point2->get_coord()[1])
        };
        SPTR(io::dicom::container::sr::dicom_srs_coord_node) scoord_node =
            std::make_shared<io::dicom::container::sr::dicom_srs_coord_node>(
                io::dicom::container::dicom_coded_attribute("121230", "DCM", "Path"),
                "INFERRED FROM",
                "POLYLINE",
                scoord_vector
            );
        num_node->add_sub_node(scoord_node);

        // Create Image Node
        SPTR(io::dicom::container::sr::dicom_sr_image_node) image_node =
            std::make_shared<io::dicom::container::sr::dicom_sr_image_node>(
                io::dicom::container::dicom_coded_attribute(),
                "SELECTED FROM",
                m_instance->get_sop_class_uid(),
                m_instance->get_sop_instance_uid_container()[frame_number1 - 1],
                int(frame_number1)
            );
        scoord_node->add_sub_node(image_node);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
