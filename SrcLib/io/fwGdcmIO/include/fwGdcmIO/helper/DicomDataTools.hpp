/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Point.hpp>

#include <gdcmImage.h>
#include <gdcmSurface.h>

namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief This class contains helpers to handle GDCM data.
 * It implements many tag value getters/setters. It defines
 * also some tools for converting GDCM data to Sight data.
 */
class FWGDCMIO_CLASS_API DicomDataTools
{
public:

    /**
     * @brief Return the pixel type of a fwData Image.
     * @param[in] image Image of fwData
     * @return The pixel format (e.g: INT8, UINT16, ...)
     */
    FWGDCMIO_API static const ::gdcm::PixelFormat getPixelType(const ::fwData::Image::csptr& image);

    /**
     * @brief Return the photometric interpretation of an acquisition.
     * @param[in] image Image
     * @return The photometric interpretation (e.g: MONOCHROME2, RGB, ...)
     */
    FWGDCMIO_API static const ::gdcm::PhotometricInterpretation getPhotometricInterpretation(
        const ::fwData::Image::csptr& image);

    /**
     * @brief Convert a surface representation mode (Sight) into recommended presentation type (DICOM).
     * @see Recommended Presentation Type Tag (0066,000d).
     * @param[in] representationMode Surface representation mode.
     * @return Corresponding recommended presentation type.
     */
    FWGDCMIO_API static ::gdcm::Surface::VIEWType convertToPresentationType(
        ::fwData::Material::RepresentationType representationMode);

    /**
     * @brief Convert a surface recommended presentation type (DICOM) into representation mode (Sight).
     * @param[in] presentationType Surface recommended presentation type.
     * @return Corresponding representation mode
     */
    FWGDCMIO_API static ::fwData::Material::RepresentationType convertToRepresentationMode(
        ::gdcm::Surface::VIEWType presentationType);

    /**
     * @brief Convert a 3D point to the closest frame number index
     * @param[in] image Image containing the point
     * @param[in] point 3D point
     */
    FWGDCMIO_API static std::size_t convertPointToFrameNumber(const ::fwData::Image::csptr& image,
                                                              const ::fwData::Point::csptr& point)
    throw(::fwGdcmIO::exception::Failed);

    /**
     * @brief Convert a frame number to a Z coordinate
     * @param[in] image Image containing the point
     * @param[in] frameNumber Frame number
     */
    FWGDCMIO_API static double convertFrameNumberToZCoordinate(const ::fwData::Image::csptr& image,
                                                               const std::size_t frameNumber)
    throw(::fwGdcmIO::exception::Failed);

};

} // namespace helper
} // namespace fwGdcmIO
