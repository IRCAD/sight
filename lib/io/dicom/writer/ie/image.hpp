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

#include <data/image.hpp>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief image Information Entity class
 */
class SIGHT_IO_DICOM_CLASS_API image : public io::dicom::writer::ie::information_entity<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] _writer GDCM writer that must be enriched
     * @param[in] _instance DICOM instance used to share information between modules
     * @param[in] _image image data
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API image(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::csptr& _image,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~image() override;

    /**
     * @brief Write General image Module tags
     * @see PS 3.3 C.7.6.1
     */
    SIGHT_IO_DICOM_API virtual void write_general_image_module();

    /**
     * @brief Write General image Module specific tags
     * @see PS 3.3 C.7.6.1
     * @param[in] _instance_number Instance number
     */
    SIGHT_IO_DICOM_API virtual void write_general_image_module_specific_tags(unsigned int _instance_number);

    /**
     * @brief Write image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    SIGHT_IO_DICOM_API virtual void write_image_plane_module();

    /**
     * @brief Write image Plane Module tags
     * @see PS 3.3 C.7.6.2
     * @param[in] _instance_number Instance number
     */
    SIGHT_IO_DICOM_API virtual void write_image_plane_module_specific_tags(unsigned int _instance_number);

    /**
     * @brief Write image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    SIGHT_IO_DICOM_API virtual void write_image_pixel_module();

    /**
     * @brief Write image Pixel Module specific tags
     * @see PS 3.3 C.7.6.3
     * @param[in] _instance_number Instance number
     */
    SIGHT_IO_DICOM_API virtual void write_image_pixel_module_specific_tags(unsigned int _instance_number);

    /**
     * @brief Write VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    SIGHT_IO_DICOM_API virtual void write_voilut_module();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    SIGHT_IO_DICOM_API virtual void write_sop_common_module();

    /**
     * @brief Write SOP Common Module specific tags
     * @see PS 3.3 C.12.1
     * @param[in] _instance_number Instance number
     */
    SIGHT_IO_DICOM_API virtual void write_sop_common_module_specific_tags(unsigned int _instance_number);

    /**
     * @brief Write CT image Module tags
     * @see PS 3.3 C.8.2.1
     */
    SIGHT_IO_DICOM_API virtual void write_ct_image_module();

    /**
     * @brief Write MR image Module tags
     * @see PS 3.3 C.8.3.1
     */
    SIGHT_IO_DICOM_API virtual void write_mr_image_module();
};

} // namespace sight::io::dicom::writer::ie
