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

#include "io/dicom/reader/tid/MeasurementReport.hpp"

#include "io/dicom/reader/tid/Fiducial.hpp"
#include "io/dicom/reader/tid/Measurement.hpp"

namespace sight::io::dicom::reader::tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(
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

MeasurementReport::~MeasurementReport()
= default;

//------------------------------------------------------------------------------

void MeasurementReport::readSR(const SPTR(io::dicom::container::sr::DicomSRNode)& _root)
{
    if(_root->getCodedAttribute()
       == io::dicom::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report"))
    {
        for(const SPTR(io::dicom::container::sr::DicomSRNode) & node : _root->getSubNodeContainer())
        {
            // Try to identify a fiducial node
            if(node->getCodedAttribute() == io::dicom::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"))
            {
                for(const SPTR(io::dicom::container::sr::DicomSRNode) & sub_node : node->getSubNodeContainer())
                {
                    io::dicom::reader::tid::Fiducial fiducial(m_dicomSeries, m_reader, m_instance, m_object, m_logger);
                    fiducial.readNode(sub_node);
                }
            }
            // Try to identify a measurement node
            else if(node->getCodedAttribute()
                    == io::dicom::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"))
            {
                for(const SPTR(io::dicom::container::sr::DicomSRNode) & sub_node : node->getSubNodeContainer())
                {
                    io::dicom::reader::tid::Measurement measurement(m_dicomSeries, m_reader, m_instance, m_object,
                                                                    m_logger);
                    measurement.readNode(sub_node);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::tid
