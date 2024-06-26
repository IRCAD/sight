/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include "io/dicom/writer/ie/information_entity.hpp"

#include <data/series.hpp>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief Frame Of Reference Information Entity class
 */
class SIGHT_IO_DICOM_CLASS_API frame_of_reference : public io::dicom::writer::ie::information_entity<data::series>
{
public:

    /**
     * @brief Constructor
     * @param[in] _writer GDCM writer that must be enriched
     * @param[in] _instance DICOM instance used to share information between modules
     * @param[in] _series Series data
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API frame_of_reference(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::series::csptr& _series,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~frame_of_reference() override;

    /**
     * @brief Write Frame of Reference Module tags
     * @see PS 3.3 C.7.4.1
     */
    SIGHT_IO_DICOM_API virtual void write_frame_of_reference_module();
};

} // namespace sight::io::dicom::writer::ie
