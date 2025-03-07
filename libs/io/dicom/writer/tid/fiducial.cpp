/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "io/dicom/writer/tid/fiducial.hpp"

#include "io/dicom/container/dicom_coded_attribute.hpp"
#include "io/dicom/container/sr/dicom_sr_code_node.hpp"
#include "io/dicom/container/sr/dicom_sr_image_node.hpp"
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

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace sight::io::dicom::writer::tid
{

//------------------------------------------------------------------------------

fiducial::fiducial(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::image::csptr& _image
) :
    io::dicom::writer::tid::template_id<data::image>(_writer, _instance, _image)
{
}

//------------------------------------------------------------------------------

fiducial::~fiducial()
= default;

//------------------------------------------------------------------------------

void fiducial::create_nodes(
    const SPTR(io::dicom::container::sr::dicom_sr_node)& _parent,
    bool _use_s_coord_3d
)
{
    data::point_list::sptr point_list = data::helper::medical_image::get_landmarks(*m_object);
    if(point_list)
    {
        unsigned int id = 1;
        for(const data::point::sptr& point : point_list->get_points())
        {
            this->create_fiducial(_parent, point, id++, _use_s_coord_3d);
        }
    }
}

//------------------------------------------------------------------------------

void fiducial::create_fiducial(
    const SPTR(io::dicom::container::sr::dicom_sr_node)& _parent,
    const data::point::csptr& _point,
    unsigned int _id,
    bool _use_s_coord_3d
)
{
    // Create Fiducial node
    SPTR(io::dicom::container::sr::dicom_sr_code_node) root_node =
        std::make_shared<io::dicom::container::sr::dicom_sr_code_node>(
            io::dicom::container::dicom_coded_attribute("122340", "DCM", "Fiducial feature"),
            "CONTAINS",
            io::dicom::container::dicom_coded_attribute("111123", "DCM", "Marker placement")
        ); //FIXME : Find a better
           // representation
    _parent->add_sub_node(root_node);

    // Create Fiducial ID node
    SPTR(io::dicom::container::sr::dicom_sruid_ref_node) id_node =
        std::make_shared<io::dicom::container::sr::dicom_sruid_ref_node>(
            io::dicom::container::dicom_coded_attribute(
                "dd1201",
                "DCM",
                "Fiducial ID"
            ),
            "HAS PROPERTIES",
            std::to_string(_id)
        );
    root_node->add_sub_node(id_node);

    // Create Fiducial UID node
    gdcm::UIDGenerator generator;
    SPTR(io::dicom::container::sr::dicom_sruid_ref_node) uid_node =
        std::make_shared<io::dicom::container::sr::dicom_sruid_ref_node>(
            io::dicom::container::dicom_coded_attribute("dd1202", "DCM", "Fiducial UID"),
            "HAS PROPERTIES",
            generator.Generate()
        );
    root_node->add_sub_node(uid_node);

    // Create Fiducial intent node
    const std::string label = _point->get_label();
    SPTR(io::dicom::container::sr::dicom_sr_text_node) intent_node =
        std::make_shared<io::dicom::container::sr::dicom_sr_text_node>(
            io::dicom::container::dicom_coded_attribute("122369", "DCM", "Fiducial intent"),
            "HAS PROPERTIES",
            label
        );
    root_node->add_sub_node(intent_node);

    if(_use_s_coord_3d)
    {
        // Create SCoord3D Node
        std::vector<float> scoord_vector {
            static_cast<float>((*_point)[0]),
            static_cast<float>((*_point)[1]),
            static_cast<float>((*_point)[2])
        };
        SPTR(io::dicom::container::sr::dicom_srs_coord3_d_node) scoord_3d_node =
            std::make_shared<io::dicom::container::sr::dicom_srs_coord3_d_node>(
                io::dicom::container::dicom_coded_attribute(),
                "HAS PROPERTIES",
                "POINT",
                scoord_vector,
                m_instance->get_sop_instance_uid_container()[0]
            );
        root_node->add_sub_node(scoord_3d_node);
    }
    else
    {
        // Create SCoord Node
        std::vector<float> scoord_vector {
            static_cast<float>((*_point)[0]),
            static_cast<float>((*_point)[1])
        };
        SPTR(io::dicom::container::sr::dicom_srs_coord_node) scoord_node =
            std::make_shared<io::dicom::container::sr::dicom_srs_coord_node>(
                io::dicom::container::dicom_coded_attribute(),
                "HAS PROPERTIES",
                "POINT",
                scoord_vector
            );
        root_node->add_sub_node(scoord_node);

        // Create Image Node
        const std::size_t frame_number = io::dicom::helper::dicom_data_tools::convert_point_to_frame_number(
            m_object,
            _point
        );
        SPTR(io::dicom::container::sr::dicom_sr_image_node) image_node =
            std::make_shared<io::dicom::container::sr::dicom_sr_image_node>(
                io::dicom::container::dicom_coded_attribute(),
                "SELECTED FROM",
                m_instance->get_sop_class_uid(),
                m_instance->get_sop_instance_uid_container()[frame_number - 1],
                int(frame_number)
            );
        scoord_node->add_sub_node(image_node);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::tid
