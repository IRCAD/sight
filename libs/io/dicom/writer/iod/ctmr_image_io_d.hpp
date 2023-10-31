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

#pragma once

#include "io/dicom/config.hpp"
#include "io/dicom/container/dicom_instance.hpp"
#include "io/dicom/writer/iod/information_object_definition.hpp"

namespace sight::io::dicom::writer::iod
{

/**
 * @brief CTMRImageIOD class used to write CTImage and MRImage DICOM files
 */
class IO_DICOM_CLASS_API ctmr_image_iod : public io::dicom::writer::iod::information_object_definition
{
public:

    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] destinationPath Destination path
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API ctmr_image_iod(
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const std::filesystem::path& _destination_path,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    IO_DICOM_API ~ctmr_image_iod() override;

    /// Write DICOM file
    IO_DICOM_API void write(const data::series::csptr& _series) override;
};

} // namespace sight::io::dicom::writer::iod
