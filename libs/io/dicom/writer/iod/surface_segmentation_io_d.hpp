/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include "io/dicom/container/dicom_instance.hpp"
#include "io/dicom/writer/iod/information_object_definition.hpp"

#include <filesystem>

namespace sight::io::dicom::writer::iod
{

/**
 * @brief SurfaceSegmentationIOD class used to write Surface Segmentation DICOM files
 */
class SIGHT_IO_DICOM_CLASS_API surface_segmentation_iod : public io::dicom::writer::iod::information_object_definition
{
public:

    /**
     * @brief Constructor
     * @param[in] _instance DICOM instance used to share information between modules
     * @param[in] _image_instance Associated image instance
     * @param[in] _destination_path Destination path
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API surface_segmentation_iod(
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        SPTR(io::dicom::container::dicom_instance)_image_instance,
        const std::filesystem::path& _destination_path,
        const core::log::logger::sptr& _logger = std::make_shared<core::log::logger>(),
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~surface_segmentation_iod() override;

    /// Write DICOM file
    SIGHT_IO_DICOM_API void write(const data::series::csptr& _series) override;

protected:

    /// Image instance
    SPTR(io::dicom::container::dicom_instance) m_image_instance;
};

} // namespace sight::io::dicom::writer::iod
