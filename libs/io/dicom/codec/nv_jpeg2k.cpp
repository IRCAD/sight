/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "nv_jpeg2k.hpp"

#include "gdcmSequenceOfFragments.h"

#include <io/bitmap/writer.hpp>

namespace sight::io::dicom::codec
{

//------------------------------------------------------------------------------

static inline core::type gdcm_to_sight_pf(const gdcm::PixelFormat& _pf)
{
    switch(_pf.GetScalarType())
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

static inline enum data::image::pixel_format_t gdcm_to_sight_pi(
    const gdcm::PhotometricInterpretation& _pi,
    const gdcm::PixelFormat& _pf
)
{
    if(_pi == gdcm::PhotometricInterpretation::PALETTE_COLOR)
    {
        // PALETTE_COLOR is always expended as RGB
        return data::image::pixel_format_t::rgb;
    }

    const auto gdcm_sample_per_pixel = _pf.GetSamplesPerPixel();

    if(gdcm_sample_per_pixel == 1)
    {
        // No need to check, no color space conversion...
        return data::image::pixel_format_t::gray_scale;
    }

    if(gdcm_sample_per_pixel == 3
       && (_pi == gdcm::PhotometricInterpretation::YBR_FULL
           || _pi == gdcm::PhotometricInterpretation::YBR_FULL_422
           || _pi == gdcm::PhotometricInterpretation::YBR_ICT
           || _pi == gdcm::PhotometricInterpretation::YBR_RCT
           || _pi == gdcm::PhotometricInterpretation::RGB))
    {
        return data::image::pixel_format_t::rgb;
    }

    // Unsupported...
    return data::image::pixel_format_t::undefined;
}

//------------------------------------------------------------------------------

bool nv_jpeg2_k::Code(gdcm::DataElement const& _in, gdcm::DataElement& _out)
{
    _out = _in;

    gdcm::SmartPointer<gdcm::SequenceOfFragments> sq = new gdcm::SequenceOfFragments;

    const auto* dims = this->GetDimensions();

    const auto* in_byte_value = _in.GetByteValue();
    const auto* in_pointer    = in_byte_value->GetPointer();
    const auto in_length      = in_byte_value->GetLength();
    const auto frame_size     = in_length / dims[2];

    // Create the image used as input buffer
    auto image           = std::make_shared<data::image>();
    const auto dump_lock = image->dump_lock();

    // Create the writer
    auto writer = std::make_shared<bitmap::writer>();
    writer->set_object(image);

    // The output buffer is resized by the writer if not big enough
    std::vector<std::uint8_t> output_buffer(frame_size);

    const auto& sight_type   = gdcm_to_sight_pf(this->GetPixelFormat());
    const auto& sight_size   = sight::data::image::size_t {dims[0], dims[1], 1};
    const auto& sight_format = gdcm_to_sight_pi(this->GetPhotometricInterpretation(), this->GetPixelFormat());

    for(std::size_t z = 0, end = dims[2] ; z < end ; ++z)
    {
        // Compute the address of the current frame
        const char* in_frame = in_pointer + (z * std::size_t(frame_size));

        // We change the buffer address of the image to avoid unneeded copy
        image->set_buffer(
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
            bitmap::backend::nvjpeg2k_j2k,
            bitmap::writer::mode::fast
        );

        SIGHT_THROW_IF("Output size is greater than 4GB", output_size > 0xFFFFFFFF);

        // Add the encoded frame to the sequence
        gdcm::Fragment frag;
        frag.SetByteValue(reinterpret_cast<char*>(output_buffer.data()), std::uint32_t(output_size));
        sq->AddFragment(frag);
    }

    _out.SetValue(*sq);

    return true;
}

//------------------------------------------------------------------------------

gdcm::ImageCodec* nv_jpeg2_k::Clone() const
{
    auto* copy = new nv_jpeg2_k;
    return copy;
}

} // namespace sight::io::dicom::codec
