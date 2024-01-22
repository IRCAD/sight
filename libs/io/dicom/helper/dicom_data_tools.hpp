/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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
#include "io/dicom/container/dicom_coded_attribute.hpp"
#include "io/dicom/exception/failed.hpp"

#include <data/image.hpp>
#include <data/material.hpp>
#include <data/point.hpp>

#include <gdcmImage.h>
#include <gdcmSurface.h>

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers to handle GDCM data.
 * It implements many tag value getters/setters. It defines
 * also some tools for converting GDCM data to Sight data.
 */
class IO_DICOM_CLASS_API dicom_data_tools
{
public:

    /**
     * @brief Return the gdcm pixel type of a sight image.
     * @param[in] _type pixel type from a sight.
     * @return The pixel format (e.g: INT8, UINT16, ...)
     */
    IO_DICOM_API static gdcm::PixelFormat get_pixel_type(const core::type& _type);

    /**
     * @brief Return the photometric interpretation of an acquisition.
     * @param[in] _image Image
     * @return The photometric interpretation (e.g: MONOCHROME2, RGB, ...)
     */
    IO_DICOM_API static gdcm::PhotometricInterpretation get_photometric_interpretation(
        const data::image::csptr& _image
    );

    /**
     * @brief Convert a surface representation mode (Sight) into recommended presentation type (DICOM).
     * @see Recommended Presentation Type Tag (0066,000d).
     * @param[in] _representation_mode Surface representation mode.
     * @return Corresponding recommended presentation type.
     */
    IO_DICOM_API static gdcm::Surface::VIEWType convert_to_presentation_type(
        data::material::representation_t _representation_mode
    );

    /**
     * @brief Convert a surface recommended presentation type (DICOM) into representation mode (Sight).
     * @param[in] _presentation_type Surface recommended presentation type.
     * @return Corresponding representation mode
     */
    IO_DICOM_API static data::material::representation_t convert_to_representation_mode(
        gdcm::Surface::VIEWType _presentation_type
    );

    /**
     * @brief Convert a 3D point to the closest frame number index
     * @param[in] _image Image containing the point
     * @param[in] _point 3D point
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API static std::size_t convert_point_to_frame_number(
        const data::image::csptr& _image,
        const data::point::csptr& _point
    );

    /**
     * @brief Convert a frame number to a Z coordinate
     * @param[in] _image Image containing the point
     * @param[in] _frame_number Frame number
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API static double convert_frame_number_to_z_coordinate(
        const data::image::csptr& _image,
        std::size_t _frame_number
    );
};

} // namespace sight::io::dicom::helper
