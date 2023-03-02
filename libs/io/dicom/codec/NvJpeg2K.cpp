/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "NvJpeg2K.hpp"

#include "gdcmSequenceOfFragments.h"

#include <io/bitmap/Writer.hpp>

namespace sight::io::dicom::codec
{

//------------------------------------------------------------------------------

core::Type gdcmToSightPf(const gdcm::PixelFormat& pf)
{
    switch(pf.GetScalarType())
    {
        case gdcm::PixelFormat::SINGLEBIT:
        case gdcm::PixelFormat::UINT8:
            return core::Type::UINT8;

        case gdcm::PixelFormat::INT8:
            return core::Type::INT8;

        case gdcm::PixelFormat::UINT16:
            return core::Type::UINT16;

        case gdcm::PixelFormat::INT16:
            return core::Type::INT16;

        case gdcm::PixelFormat::UINT32:
            return core::Type::UINT32;

        case gdcm::PixelFormat::INT32:
            return core::Type::INT32;

        case gdcm::PixelFormat::UINT64:
            return core::Type::UINT64;

        case gdcm::PixelFormat::INT64:
            return core::Type::INT64;

        case gdcm::PixelFormat::FLOAT32:
            return core::Type::FLOAT;

        case gdcm::PixelFormat::FLOAT64:
            return core::Type::DOUBLE;

        default:
            return core::Type::NONE;
    }
}

//------------------------------------------------------------------------------

data::Image::PixelFormat gdcmToSightPi(
    const gdcm::PhotometricInterpretation& pi,
    const gdcm::PixelFormat& pf
)
{
    if(pi == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // PALETTE_COLOR is always expended as RGB
        return data::Image::PixelFormat::RGB;
    }

    const auto gdcm_sample_per_pixel = pf.GetSamplesPerPixel();

    if(gdcm_sample_per_pixel == 1)
    {
        // No need to check, no color space conversion...
        return data::Image::PixelFormat::GRAY_SCALE;
    }

    if(gdcm_sample_per_pixel == 3
       && (pi == gdcm::PhotometricInterpretation::YBR_FULL
           || pi == gdcm::PhotometricInterpretation::YBR_FULL_422
           || pi == gdcm::PhotometricInterpretation::YBR_ICT
           || pi == gdcm::PhotometricInterpretation::YBR_RCT
           || pi == gdcm::PhotometricInterpretation::RGB))
    {
        return data::Image::PixelFormat::RGB;
    }

    SIGHT_THROW_IF(
        "Retired photometric interpretation used in DICOM file.",
        pi == gdcm::PhotometricInterpretation::ARGB
        || pi == gdcm::PhotometricInterpretation::CMYK
        || pi == gdcm::PhotometricInterpretation::HSV
        || pi == gdcm::PhotometricInterpretation::YBR_PARTIAL_420
        || pi == gdcm::PhotometricInterpretation::YBR_PARTIAL_422
    );

    // Unsupported...
    return data::Image::PixelFormat::UNDEFINED;
}

//------------------------------------------------------------------------------

bool NvJpeg2K::Code(gdcm::DataElement const& in, gdcm::DataElement& out)
{
    out = in;

    gdcm::SmartPointer<gdcm::SequenceOfFragments> sq = new gdcm::SequenceOfFragments;

    const unsigned int* dims = this->GetDimensions();

    const gdcm::ByteValue* bv = in.GetByteValue();
    const char* input         = bv->GetPointer();
    std::uint64_t length      = bv->GetLength();
    std::uint64_t imageLength = length / dims[2];
    size_t inputLength        = imageLength;

    for(unsigned int dim = 0 ; dim < dims[2] ; ++dim)
    {
        const char* inputData = input + dim * imageLength;

        auto writer = bitmap::Writer::New();
        auto image  = data::Image::New();
        std::array<std::size_t, 3> size {dims[0], dims[1], 1};
        image->resize(
            size,
            gdcmToSightPf(this->GetPixelFormat()),
            gdcmToSightPi(this->GetPhotometricInterpretation(), this->GetPixelFormat())
        );
        std::copy_n(inputData, inputLength, static_cast<char*>(image->getBufferObject()->getBuffer()));
        writer->setObject(image);
        std::stringstream ss;
        try
        {
            writer->write(ss, bitmap::Backend::NVJPEG2K_J2K);
        }
        catch(...)
        {
            return false;
        }
        auto* byteValue = new char [ss.str().size()];
        std::copy_n(ss.str().data(), ss.str().size(), byteValue);
        gdcm::Fragment frag;
        frag.SetByteValue(byteValue, static_cast<std::uint32_t>(ss.str().size()));
        sq->AddFragment(frag);
    }

    out.SetValue(*sq);

    return true;
}

//------------------------------------------------------------------------------

gdcm::ImageCodec* NvJpeg2K::Clone() const
{
    auto* copy = new NvJpeg2K;
    return copy;
}

} // namespace sight::io::dicom::codec
