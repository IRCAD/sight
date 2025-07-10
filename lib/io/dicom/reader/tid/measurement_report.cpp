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

#include "io/dicom/reader/tid/measurement_report.hpp"

#include "io/dicom/reader/tid/fiducial.hpp"
#include "io/dicom/reader/tid/measurement.hpp"

namespace sight::io::dicom::reader::tid
{

//------------------------------------------------------------------------------

measurement_report::measurement_report(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::image::sptr& _image,
    const core::log::logger::sptr& _logger
) :
    io::dicom::reader::tid::template_id<data::image>(_dicom_series, _reader, _instance, _image, _logger)
{
}

//------------------------------------------------------------------------------

measurement_report::~measurement_report()
= default;

//------------------------------------------------------------------------------

void measurement_report::read_sr(const SPTR(io::dicom::container::sr::dicom_sr_node)& _root)
{
    if(_root->get_coded_attribute()
       == io::dicom::container::dicom_coded_attribute("dd1dd1", "DCM", "Imaging Measurement Report"))
    {
        for(const SPTR(io::dicom::container::sr::dicom_sr_node) & node : _root->get_sub_node_container())
        {
            // Try to identify a fiducial node
            if(node->get_coded_attribute() == io::dicom::container::dicom_coded_attribute("dd1d93", "DCM", "Fiducials"))
            {
                for(const SPTR(io::dicom::container::sr::dicom_sr_node) & sub_node : node->get_sub_node_container())
                {
                    io::dicom::reader::tid::fiducial fiducial(m_dicom_series, m_reader, m_instance, m_object, m_logger);
                    fiducial.read_node(sub_node);
                }
            }
            // Try to identify a measurement node
            else if(node->get_coded_attribute()
                    == io::dicom::container::dicom_coded_attribute("dd1d91", "DCM", "Imaging Measurements"))
            {
                for(const SPTR(io::dicom::container::sr::dicom_sr_node) & sub_node : node->get_sub_node_container())
                {
                    io::dicom::reader::tid::measurement measurement(m_dicom_series, m_reader, m_instance, m_object,
                                                                    m_logger);
                    measurement.read_node(sub_node);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::tid
