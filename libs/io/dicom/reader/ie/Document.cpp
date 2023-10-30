/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/dicom/reader/ie/Document.hpp"

#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRContainerNode.hpp"
#include "io/dicom/helper/StructuredReport.hpp"
#include "io/dicom/reader/tid/MeasurementReport.hpp"

namespace sight::io::dicom::reader::ie
{

//------------------------------------------------------------------------------

document::document(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(gdcm::Reader)& _reader,
    const io::dicom::container::dicom_instance::sptr& _instance,
    const data::image::sptr& _image,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::ie::information_entity<data::image>(_dicom_series, _reader, _instance, _image,
                                                           _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

document::~document()
= default;

//------------------------------------------------------------------------------

void document::read_sr()
{
    // Retrieve dataset
    const gdcm::DataSet& dataset_root = m_reader->GetFile().GetDataSet();

    // Create SR from Dataset
    SPTR(io::dicom::container::sr::dicom_sr_container_node) root_container_node =
        io::dicom::helper::structured_report::read_sr(dataset_root);

    if(root_container_node)
    {
        // Try to read a measurement report
        io::dicom::reader::tid::measurement_report report(m_dicom_series, m_reader, m_instance, m_object, m_logger);
        report.read_sr(root_container_node);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::ie
