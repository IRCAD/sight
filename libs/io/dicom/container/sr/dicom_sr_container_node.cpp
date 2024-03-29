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

#include "io/dicom/container/sr/dicom_sr_container_node.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

dicom_sr_container_node::dicom_sr_container_node(
    const dicom_coded_attribute& _coded_attribute,
    const std::string& _relationship
) :
    io::dicom::container::sr::dicom_sr_node(_coded_attribute, "CONTAINER", _relationship)
{
}

//------------------------------------------------------------------------------

dicom_sr_container_node::~dicom_sr_container_node()
= default;

//------------------------------------------------------------------------------

void dicom_sr_container_node::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::dicom_sr_node::write(_dataset);

    // Continuity of content - Type 1 - See PS 3.3 C.18.8 (Only for type CONTAINER)
    // NOTE : Continuity is fixed to SEPARATE because it provides a better compatibility.
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0xa050>("SEPARATE", _dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
