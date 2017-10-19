/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMDATATOOLS_HPP__
#define __FWGDCMIO_HELPER_DICOMDATATOOLS_HPP__

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
 * also some tools for converting GDCM data to FW4SPL data.
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
     * @brief Convert a surface representation mode (FW4SPL) into recommended presentation type (DICOM).
     * @see Recommended Presentation Type Tag (0066,000d).
     * @param[in] representationMode Surface representation mode.
     * @return Corresponding recommended presentation type.
     */
    FWGDCMIO_API static ::gdcm::Surface::VIEWType convertToPresentationType(
            ::fwData::Material::RepresentationType representationMode);

    /**
     * @brief Convert a surface recommended presentation type (DICOM) into representation mode (FW4SPL).
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
                                                              const ::fwData::Point::sptr& point)
                                                              throw(::fwGdcmIO::exception::Failed);

    /**
     * @brief Convert a frame number to a Z coordinate
     * @param[in] image Image containing the point
     * @param[in] frameNumber Frame number
     */
    FWGDCMIO_API static double convertFrameNumberToZCoordinate(const ::fwData::Image::csptr& image,
                                                               std::size_t frameNumber)
                                                               throw(::fwGdcmIO::exception::Failed);

};

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_DICOMDATATOOLS_HPP__

