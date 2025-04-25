/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "io/dicom/container/sr/dicom_srs_coord3dnode.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

#include <service/macros.hpp>

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

dicom_srs_coord3_d_node::dicom_srs_coord3_d_node(
    const dicom_coded_attribute& _coded_attribute,
    const std::string& _relationship,
    std::string _graphic_type,
    graphic_data_container_t _graphic_data_container,
    std::string _frame_of_reference_uid
) :
    io::dicom::container::sr::dicom_sr_node(_coded_attribute, "SCOORD3D", _relationship),
    m_frame_of_reference_uid(std::move(_frame_of_reference_uid)),
    m_graphic_type(std::move(_graphic_type)),
    m_graphic_data_container(std::move(_graphic_data_container))
{
    SIGHT_ASSERT(
        "Only POINT and POLYLINE are supported by SCoord3D node for now.",
        m_graphic_type == "POINT" || m_graphic_type == "POLYLINE"
    );

    SIGHT_ASSERT(
        "Graphic data doesn't match graphic type POINT.",
        m_graphic_type != "POINT" || m_graphic_data_container.size() == 3
    );

    SIGHT_ASSERT(
        "Graphic data doesn't match graphic type POLYLINE.",
        m_graphic_type != "POLYLINE" || m_graphic_data_container.size() == 6
    );
}

//------------------------------------------------------------------------------

dicom_srs_coord3_d_node::~dicom_srs_coord3_d_node()
= default;

//------------------------------------------------------------------------------

void dicom_srs_coord3_d_node::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::dicom_sr_node::write(_dataset);

    // Referenced Frame of Reference UID - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x3006, 0x0024>(m_frame_of_reference_uid, _dataset);

    // Graphic Data - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_values<float, 0x0070, 0x0022>(
        (m_graphic_data_container).data(),
        static_cast<unsigned int>(m_graphic_data_container.size()),
        _dataset
    );

    // Graphic Type - Type 1
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0070, 0x0023>(m_graphic_type, _dataset);
}

//------------------------------------------------------------------------------

void dicom_srs_coord3_d_node::print(std::ostream& _os) const
{
    dicom_sr_node::print(_os);
    _os << "\\nFrame of Reference UID : [" << m_frame_of_reference_uid << "]";
    _os << "\\nGraphic Type : [" << m_graphic_type << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
