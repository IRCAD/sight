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

#include "io/dicom/container/sr/DicomSRUIDRefNode.hpp"

#include "io/dicom/helper/DicomDataWriter.hxx"

#include <utility>

namespace sight::io::dicom::container::sr
{

//------------------------------------------------------------------------------

dicom_sruid_ref_node::dicom_sruid_ref_node(
    const dicom_coded_attribute& _coded_attribute,
    const std::string& _relationship,
    std::string _uid_value
) :
    io::dicom::container::sr::dicom_sr_node(_coded_attribute, "UIDREF", _relationship),
    m_uid_value(std::move(_uid_value))
{
}

//------------------------------------------------------------------------------

dicom_sruid_ref_node::~dicom_sruid_ref_node()
= default;

//------------------------------------------------------------------------------

void dicom_sruid_ref_node::write(gdcm::DataSet& _dataset) const
{
    io::dicom::container::sr::dicom_sr_node::write(_dataset);

    // UID Value - Type 1C
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0040, 0xa124>(m_uid_value, _dataset);
}

//------------------------------------------------------------------------------

void dicom_sruid_ref_node::print(std::ostream& _os) const
{
    dicom_sr_node::print(_os);
    _os << "\\nUID value : [" << m_uid_value << "]";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::sr
