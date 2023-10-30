/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dicom/reader/iod/ComprehensiveSRIOD.hpp"

#include "io/dicom/reader/ie/Document.hpp"

#include <data/image_series.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

#include <memory>
#include <utility>

namespace sight::io::dicom::reader::iod
{

//------------------------------------------------------------------------------

comprehensive_sriod::comprehensive_sriod(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::reader::iod::information_object_definition(_dicom_series, _instance, _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

comprehensive_sriod::~comprehensive_sriod()
= default;

//------------------------------------------------------------------------------

void comprehensive_sriod::read(data::series::sptr _series)
{
    // Retrieve image series
    data::image_series::sptr image_series = std::dynamic_pointer_cast<data::image_series>(_series);
    SIGHT_ASSERT("Image series should not be null.", image_series);

    // Create GDCM reader
    SPTR(gdcm::Reader) reader = std::make_shared<gdcm::Reader>();

    // Read the first file
    const auto& dicom_container                                 = m_dicom_series->get_dicom_container();
    const core::memory::buffer_object::sptr buffer_obj          = dicom_container.begin()->second;
    const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
    SPTR(std::istream) is = stream_info.stream;
    reader->SetStream(*is);
    const bool success = reader->Read();

    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::failed(
            "Unable to read the DICOM instance \""
            + buffer_obj->get_stream_info().fs_file.string()
            + "\" using the GDCM Image Reader."
        ),
        !success
    );

    // Create Information Entity helpers
    io::dicom::reader::ie::document document_ie(m_dicom_series, reader, m_instance, image_series, m_logger,
                                                m_progress_callback, m_cancel_requested_callback);

    // Read SR
    document_ie.read_sr();
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::iod
