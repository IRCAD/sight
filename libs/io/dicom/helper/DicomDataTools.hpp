/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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
#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/exception/Failed.hpp"

#include <data/Image.hpp>
#include <data/Material.hpp>
#include <data/Point.hpp>

#include <gdcmImage.h>
#include <gdcmSurface.h>

namespace sight::io::dicom
{

namespace helper
{

/**
 * @brief This class contains helpers to handle GDCM data.
 * It implements many tag value getters/setters. It defines
 * also some tools for converting GDCM data to Sight data.
 */
class IO_DICOM_CLASS_API DicomDataTools
{
public:

    /**
     * @brief Return the gdcm pixel type of a sight image.
     * @param[in] type pixel type from a sight.
     * @return The pixel format (e.g: INT8, UINT16, ...)
     */
    IO_DICOM_API static const gdcm::PixelFormat getPixelType(const core::tools::Type& type);

    /**
     * @brief Return the photometric interpretation of an acquisition.
     * @param[in] image Image
     * @return The photometric interpretation (e.g: MONOCHROME2, RGB, ...)
     */
    IO_DICOM_API static const gdcm::PhotometricInterpretation getPhotometricInterpretation(
        const data::Image::csptr& image
    );

    /**
     * @brief Convert a surface representation mode (Sight) into recommended presentation type (DICOM).
     * @see Recommended Presentation Type Tag (0066,000d).
     * @param[in] representationMode Surface representation mode.
     * @return Corresponding recommended presentation type.
     */
    IO_DICOM_API static gdcm::Surface::VIEWType convertToPresentationType(
        data::Material::RepresentationType representationMode
    );

    /**
     * @brief Convert a surface recommended presentation type (DICOM) into representation mode (Sight).
     * @param[in] presentationType Surface recommended presentation type.
     * @return Corresponding representation mode
     */
    IO_DICOM_API static data::Material::RepresentationType convertToRepresentationMode(
        gdcm::Surface::VIEWType presentationType
    );

    /**
     * @brief Convert a 3D point to the closest frame number index
     * @param[in] image Image containing the point
     * @param[in] point 3D point
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API static std::size_t convertPointToFrameNumber(
        const data::Image::csptr& image,
        const data::Point::csptr& point
    );

    /**
     * @brief Convert a frame number to a Z coordinate
     * @param[in] image Image containing the point
     * @param[in] frameNumber Frame number
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API static double convertFrameNumberToZCoordinate(
        const data::Image::csptr& image,
        const std::size_t frameNumber
    );
};

} // namespace helper

} // namespace sight::io::dicom
