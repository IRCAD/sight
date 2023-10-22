/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dicom/helper/DicomDataTools.hpp"

#include "io/dicom/helper/Encoding.hpp"

#include <core/base.hpp>

#include <gdcmGlobal.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmPixelFormat.h>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

using PixelTypeConversionMapType = std::map<core::type, gdcm::PixelFormat::ScalarType>;

static const PixelTypeConversionMapType s_PIXEL_TYPE_CONVERSION_MAP = {
    {core::type::UINT8, gdcm::PixelFormat::UINT8},
    {core::type::INT8, gdcm::PixelFormat::INT8},
    // {core::type::XXX    , gdcm::PixelFormat::UINT12}  , // Unsupported by VTK Render
    // {core::type::XXX    , gdcm::PixelFormat::INT12}   , // Unsupported by VTK Render
    {core::type::UINT16, gdcm::PixelFormat::UINT16},
    {core::type::INT16, gdcm::PixelFormat::INT16},
    {core::type::UINT32, gdcm::PixelFormat::UINT32},
    {core::type::INT32, gdcm::PixelFormat::INT32},
    // { core::type::XXX   , gdcm::PixelFormat::FLOAT16} , // Unsupported by VTK Render
    {core::type::FLOAT, gdcm::PixelFormat::FLOAT32},
    {core::type::DOUBLE, gdcm::PixelFormat::FLOAT64}
};

//------------------------------------------------------------------------------

gdcm::PixelFormat DicomDataTools::get_pixel_type(const core::type& _type)
{
    auto it = s_PIXEL_TYPE_CONVERSION_MAP.find(_type);
    if(it != s_PIXEL_TYPE_CONVERSION_MAP.end())
    {
        return it->second;
    }

    return gdcm::PixelFormat::UNKNOWN;
}

//------------------------------------------------------------------------------

gdcm::PhotometricInterpretation DicomDataTools::getPhotometricInterpretation(const data::image::csptr& _image)
{
    gdcm::PhotometricInterpretation pi;
    const std::size_t components = _image->numComponents();

    // Attempt a guess (VTK do the same choice)
    switch(components)
    {
        case 1: // It could well be MONOCHROME1
            pi = gdcm::PhotometricInterpretation::MONOCHROME2;
            break;

        case 3: // It could well be YBR
            pi = gdcm::PhotometricInterpretation::RGB;
            break;

        case 4: // It could well be CMYK
            pi = gdcm::PhotometricInterpretation::ARGB;
            break;

        default:
            SIGHT_ERROR("Photometric interpretation not found");
            pi = gdcm::PhotometricInterpretation::UNKNOWN;
            break;
    }

    return pi;
}

//------------------------------------------------------------------------------

gdcm::Surface::VIEWType DicomDataTools::convertToPresentationType(
    data::material::representation_t _representation_mode
)
{
    switch(_representation_mode)
    {
        case data::material::SURFACE:
            return gdcm::Surface::SURFACE;

        case data::material::POINT:
            return gdcm::Surface::POINTS;

        case data::material::WIREFRAME:
            return gdcm::Surface::WIREFRAME;

        default:
            SIGHT_WARN("Representation type not handle (changed to : SURFACE)");
            return gdcm::Surface::SURFACE;
    }
}

//------------------------------------------------------------------------------

data::material::representation_t DicomDataTools::convertToRepresentationMode(
    gdcm::Surface::VIEWType _presentation_type
)
{
    switch(_presentation_type)
    {
        case gdcm::Surface::SURFACE:
            return data::material::SURFACE;

        case gdcm::Surface::WIREFRAME:
            return data::material::WIREFRAME;

        case gdcm::Surface::POINTS:
            return data::material::POINT;

        default:
            SIGHT_WARN("Presentation type not handle (changed to : SURFACE)");
            return data::material::SURFACE;
    }
}

//------------------------------------------------------------------------------

std::size_t DicomDataTools::convertPointToFrameNumber(
    const data::image::csptr& _image,
    const data::point::csptr& _point
)
{
    // Retrieve Z spacing
    const double z_spacing = (_image->numDimensions() > 2) ? (_image->getSpacing()[2]) : 1;

    // Retrieve Z coordinate of image origin
    const double z_origin = (_image->numDimensions() > 2) ? (_image->getOrigin()[2]) : 0;

    // Retrieve Z coordinate
    const auto z_coordinate = static_cast<double>(_point->getCoord()[2]);

    // Compute frame number
    const std::size_t frame_number = static_cast<std::size_t>(floor((z_coordinate - z_origin) / z_spacing + 0.5)) + 1;
    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::Failed("Coordinates out of image bounds."),
        frame_number<1 || frame_number> _image->size()[2]
    );

    return frame_number;
}

//------------------------------------------------------------------------------

double DicomDataTools::convertFrameNumberToZCoordinate(
    const data::image::csptr& _image,
    const std::size_t _frame_number
)
{
    // Retrieve Z spacing
    const double z_spacing = (_image->numDimensions() > 2) ? (_image->getSpacing()[2]) : 1;

    // Retrieve Z coordinate of image origin
    const double z_origin = (_image->numDimensions() > 2) ? (_image->getOrigin()[2]) : 0;

    // Compute coordinate
    const std::size_t frame_index = (_frame_number - 1);
    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::Failed("Coordinates out of image bounds."),
        frame_index >= _image->size()[2]
    );
    const double z_coordinate = z_origin + static_cast<double>(frame_index) * z_spacing;

    return z_coordinate;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
