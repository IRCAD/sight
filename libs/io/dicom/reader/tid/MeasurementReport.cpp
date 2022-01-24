/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

namespace sight::io::dicom
{

namespace reader
{

namespace tid
{

//------------------------------------------------------------------------------

MeasurementReport::MeasurementReport(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Image::sptr& image,
    const core::log::Logger::sptr& logger
) :
    io::dicom::reader::tid::TemplateID<data::Image>(dicomSeries, reader, instance, image, logger)
{
}

//------------------------------------------------------------------------------

MeasurementReport::~MeasurementReport()
{
}

//------------------------------------------------------------------------------

void MeasurementReport::readSR(const SPTR(io::dicom::container::sr::DicomSRNode)& root)
{
    if(root->getCodedAttribute()
       == io::dicom::container::DicomCodedAttribute("dd1dd1", "DCM", "Imaging Measurement Report"))
    {
        for(const SPTR(io::dicom::container::sr::DicomSRNode) & node : root->getSubNodeContainer())
        {
            // Try to identify a fiducial node
            if(node->getCodedAttribute() == io::dicom::container::DicomCodedAttribute("dd1d93", "DCM", "Fiducials"))
            {
                for(const SPTR(io::dicom::container::sr::DicomSRNode) & subNode : node->getSubNodeContainer())
                {
                    io::dicom::reader::tid::Fiducial fiducial(m_dicomSeries, m_reader, m_instance, m_object, m_logger);
                    fiducial.readNode(subNode);
                }
            }
            // Try to identify a measurement node
            else if(node->getCodedAttribute()
                    == io::dicom::container::DicomCodedAttribute("dd1d91", "DCM", "Imaging Measurements"))
            {
                for(const SPTR(io::dicom::container::sr::DicomSRNode) & subNode : node->getSubNodeContainer())
                {
                    io::dicom::reader::tid::Measurement measurement(m_dicomSeries, m_reader, m_instance, m_object,
                                                                    m_logger);
                    measurement.readNode(subNode);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace tid

} // namespace reader

} // namespace sight::io::dicom
