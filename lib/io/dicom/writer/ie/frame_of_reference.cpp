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

#include "io/dicom/writer/ie/frame_of_reference.hpp"

#include "io/dicom/helper/dicom_data_writer.hxx"

#include <data/series.hpp>

namespace sight::io::dicom::writer::ie
{

//------------------------------------------------------------------------------

frame_of_reference::frame_of_reference(
    const SPTR(gdcm::Writer)& _writer,
    const SPTR(io::dicom::container::dicom_instance)& _instance,
    const data::series::csptr& _series,
    const core::log::logger::sptr& _logger,
    progress_callback _progress,
    cancel_requested_callback _cancel
) :
    io::dicom::writer::ie::information_entity<data::series>(_writer, _instance, _series,
                                                            _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

frame_of_reference::~frame_of_reference()
= default;

//------------------------------------------------------------------------------

void frame_of_reference::write_frame_of_reference_module()
{
    // Retrieve dataset
    gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Frame of Reference UID
    const std::string frame_of_reference_uid = m_instance->get_frame_of_reference_uid();
    io::dicom::helper::dicom_data_writer::set_tag_value<0x0020, 0x0052>(frame_of_reference_uid, dataset);

    // Position Reference Indicator - Type 2
    io::dicom::helper::dicom_data_writer::set_empty_tag_value<0x0020, 0x1040>(dataset);
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer::ie
