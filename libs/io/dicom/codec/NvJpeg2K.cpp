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

static inline core::type gdcmToSightPf(const gdcm::PixelFormat& pf)
{
    switch(pf.GetScalarType())
    {
        case gdcm::PixelFormat::SINGLEBIT:
        case gdcm::PixelFormat::UINT8:
            return core::type::UINT8;

        case gdcm::PixelFormat::INT8:
            return core::type::INT8;

        case gdcm::PixelFormat::UINT16:
            return core::type::UINT16;

        case gdcm::PixelFormat::INT16:
            return core::type::INT16;

        case gdcm::PixelFormat::UINT32:
            return core::type::UINT32;

        case gdcm::PixelFormat::INT32:
            return core::type::INT32;

        case gdcm::PixelFormat::UINT64:
            return core::type::UINT64;

        case gdcm::PixelFormat::INT64:
            return core::type::INT64;

        case gdcm::PixelFormat::FLOAT32:
            return core::type::FLOAT;

        case gdcm::PixelFormat::FLOAT64:
            return core::type::DOUBLE;

        default:
            return core::type::NONE;
    }
}

//------------------------------------------------------------------------------

static inline data::image::PixelFormat gdcmToSightPi(
    const gdcm::PhotometricInterpretation& pi,
    const gdcm::PixelFormat& pf
)
{
    if(pi == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // PALETTE_COLOR is always expended as RGB
        return data::image::PixelFormat::RGB;
    }

    const auto gdcm_sample_per_pixel = pf.GetSamplesPerPixel();

    if(gdcm_sample_per_pixel == 1)
    {
        // No need to check, no color space conversion...
        return data::image::PixelFormat::GRAY_SCALE;
    }

    if(gdcm_sample_per_pixel == 3
       && (pi == gdcm::PhotometricInterpretation::YBR_FULL
           || pi == gdcm::PhotometricInterpretation::YBR_FULL_422
           || pi == gdcm::PhotometricInterpretation::YBR_ICT
           || pi == gdcm::PhotometricInterpretation::YBR_RCT
           || pi == gdcm::PhotometricInterpretation::RGB))
    {
        return data::image::PixelFormat::RGB;
    }

    // Unsupported...
    return data::image::PixelFormat::UNDEFINED;
}

//------------------------------------------------------------------------------

bool NvJpeg2K::Code(gdcm::DataElement const& in, gdcm::DataElement& out)
{
    out = in;

    gdcm::SmartPointer<gdcm::SequenceOfFragments> sq = new gdcm::SequenceOfFragments;

    const auto* dims = this->GetDimensions();

    const auto* in_byte_value = in.GetByteValue();
    const auto* in_pointer    = in_byte_value->GetPointer();
    const auto in_length      = in_byte_value->GetLength();
    const auto frame_size     = in_length / dims[2];

    // Create the image used as input buffer
    auto image           = std::make_shared<data::image>();
    const auto dump_lock = image->dump_lock();

    // Create the writer
    auto writer = std::make_shared<bitmap::Writer>();
    writer->set_object(image);

    // The output buffer is resized by the writer if not big enough
    std::vector<std::uint8_t> output_buffer(frame_size);

    const auto& sight_type   = gdcmToSightPf(this->GetPixelFormat());
    const auto& sight_size   = sight::data::image::Size {dims[0], dims[1], 1};
    const auto& sight_format = gdcmToSightPi(this->GetPhotometricInterpretation(), this->GetPixelFormat());

    for(std::size_t z = 0, end = dims[2] ; z < end ; ++z)
    {
        // Compute the address of the current frame
        const char* in_frame = in_pointer + (z * std::size_t(frame_size));

        // We change the buffer address of the image to avoid unneeded copy
        image->setBuffer(
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
            const_cast<char*>(in_frame),
            false,
            sight_type,
            sight_size,
            sight_format,
            std::make_shared<core::memory::buffer_no_alloc_policy>()
        );

        // Encode the frame
        const auto output_size = writer->write(
            output_buffer,
            bitmap::Backend::NVJPEG2K_J2K,
            bitmap::Writer::Mode::FAST
        );

        SIGHT_THROW_IF("Output size is greater than 4GB", output_size > 0xFFFFFFFF);

        // Add the encoded frame to the sequence
        gdcm::Fragment frag;
        frag.SetByteValue(reinterpret_cast<char*>(output_buffer.data()), std::uint32_t(output_size));
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
