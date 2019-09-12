/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGdcmIO/helper/DicomDataTools.hpp"

#include "fwGdcmIO/helper/Encoding.hpp"

#include <fwCore/base.hpp>

#include <fwTools/IntrinsicTypes.hpp>

#include <gdcmGlobal.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmPixelFormat.h>

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

typedef std::map< ::fwTools::Type, ::gdcm::PixelFormat::ScalarType > PixelTypeConversionMapType;

static const PixelTypeConversionMapType s_PIXEL_TYPE_CONVERSION_MAP = {
    {::fwTools::Type::create("uint8"), ::gdcm::PixelFormat::UINT8},
    {::fwTools::Type::create("int8"), ::gdcm::PixelFormat::INT8},
    // {::fwTools::Type::create("XXX")    , ::gdcm::PixelFormat::UINT12}  , // Unsupported by VTK Render
    // {::fwTools::Type::create("XXX")    , ::gdcm::PixelFormat::INT12}   , // Unsupported by VTK Render
    {::fwTools::Type::create("uint16"), ::gdcm::PixelFormat::UINT16},
    {::fwTools::Type::create("int16"), ::gdcm::PixelFormat::INT16},
    {::fwTools::Type::create("uint32"), ::gdcm::PixelFormat::UINT32},
    {::fwTools::Type::create("int32"), ::gdcm::PixelFormat::INT32},
    // { ::fwTools::Type::create("XXX")   , ::gdcm::PixelFormat::FLOAT16} , // Unsupported by VTK Render
    {::fwTools::Type::create("float"), ::gdcm::PixelFormat::FLOAT32},
    {::fwTools::Type::create("double"), ::gdcm::PixelFormat::FLOAT64}
};

//------------------------------------------------------------------------------

const ::gdcm::PixelFormat DicomDataTools::getPixelType(const ::fwData::Image::csptr& image)
{
    auto it = s_PIXEL_TYPE_CONVERSION_MAP.find(image->getType());
    if(it != s_PIXEL_TYPE_CONVERSION_MAP.end())
    {
        return it->second;
    }
    return ::gdcm::PixelFormat::UNKNOWN;
}

//------------------------------------------------------------------------------

const ::gdcm::PhotometricInterpretation
DicomDataTools::getPhotometricInterpretation(const ::fwData::Image::csptr& image)
{
    ::gdcm::PhotometricInterpretation pi;
    const size_t components = image->getNumberOfComponents();

    // Attempt a guess (VTK do the same choice)
    switch (components)
    {
        case 1: // It could well be MONOCHROME1
            pi = ::gdcm::PhotometricInterpretation::MONOCHROME2;
            break;
        case 3: // It could well be YBR
            pi = ::gdcm::PhotometricInterpretation::RGB;
            break;
        case 4: // It could well be CMYK
            pi = ::gdcm::PhotometricInterpretation::ARGB;
            break;
        default:
            SLM_ERROR("Photometric interpretation not found");
            pi = ::gdcm::PhotometricInterpretation::UNKNOWN;
            break;
    }

    return pi;
}

//------------------------------------------------------------------------------

::gdcm::Surface::VIEWType DicomDataTools::convertToPresentationType(
    ::fwData::Material::RepresentationType representationMode)
{
    switch (representationMode)
    {
        case ::fwData::Material::SURFACE:
            return ::gdcm::Surface::SURFACE;
        case ::fwData::Material::POINT:
            return ::gdcm::Surface::POINTS;
        case ::fwData::Material::WIREFRAME:
            return ::gdcm::Surface::WIREFRAME;
        default:
            SLM_WARN("Representation type not handle (changed to : SURFACE)");
            return ::gdcm::Surface::SURFACE;
    }
}

//------------------------------------------------------------------------------

::fwData::Material::RepresentationType DicomDataTools::convertToRepresentationMode(
    ::gdcm::Surface::VIEWType presentationType)
{
    switch (presentationType)
    {
        case ::gdcm::Surface::SURFACE:
            return ::fwData::Material::SURFACE;
        case ::gdcm::Surface::WIREFRAME:
            return ::fwData::Material::WIREFRAME;
        case ::gdcm::Surface::POINTS:
            return ::fwData::Material::POINT;
        default:
            SLM_WARN("Presentation type not handle (changed to : SURFACE)");
            return ::fwData::Material::SURFACE;
    }
}

//------------------------------------------------------------------------------

std::size_t DicomDataTools::convertPointToFrameNumber(const ::fwData::Image::csptr& image,
                                                      const ::fwData::Point::csptr& point)
{
    // Retrieve Z spacing
    const double zSpacing = (image->getNumberOfDimensions() > 2) ? (image->getSpacing()[2]) : 1;

    // Retrieve Z coordinate of image origin
    const double zOrigin = (image->getNumberOfDimensions() > 2) ? (image->getOrigin()[2]) : 0;

    // Retrieve Z coordinate
    const double zCoordinate = static_cast<double>(point->getCoord()[2]);

    // Compute frame number
    const std::size_t frameNumber = static_cast<std::size_t>(floor((zCoordinate - zOrigin) / zSpacing + 0.5)) + 1;
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Coordinates out of image bounds."),
                          frameNumber < 1 || frameNumber > image->getSize()[2]);

    return frameNumber;
}

//------------------------------------------------------------------------------

double DicomDataTools::convertFrameNumberToZCoordinate(const ::fwData::Image::csptr& image,
                                                       const std::size_t frameNumber)
{
    // Retrieve Z spacing
    const double zSpacing = (image->getNumberOfDimensions() > 2) ? (image->getSpacing()[2]) : 1;

    // Retrieve Z coordinate of image origin
    const double zOrigin = (image->getNumberOfDimensions() > 2) ? (image->getOrigin()[2]) : 0;

    // Compute coordinate
    const std::size_t frameIndex = (frameNumber-1);
    FW_RAISE_EXCEPTION_IF(::fwGdcmIO::exception::Failed("Coordinates out of image bounds."),
                          frameIndex >= image->getSize()[2]);
    const double zCoordinate = zOrigin + static_cast<double>(frameIndex) * zSpacing;

    return zCoordinate;
}

//------------------------------------------------------------------------------

} //namespace helper
} //namespace fwGdcmIO
