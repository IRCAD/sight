/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#pragma once

#include <data/helper/medical_image.hpp>

#include <itkImage.h>
#include <itkImageRegion.h>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

template<class ITKIMAGE>
void move_from_itk(
    typename ITKIMAGE::Pointer _itk_image,
    sight::data::image& _image,
    bool _buffer_manager_is_data_image
)
{
    // Add by arnaud
    std::uint8_t dim                 = ITKIMAGE::ImageDimension;
    data::image::spacing_t v_spacing = {0., 0., 0.};
    data::image::origin_t v_origin   = {0., 0., 0.};
    data::image::size_t v_size       = {0, 0, 0};

    for(std::uint8_t d = 0 ; d < dim ; ++d)
    {
        v_origin[d]  = _itk_image->GetOrigin()[d];
        v_size[d]    = _itk_image->GetBufferedRegion().GetSize()[d];
        v_spacing[d] = _itk_image->GetSpacing()[d];
    }

    _image.set_origin(v_origin);
    _image.set_spacing(v_spacing);

    const auto& direction = _itk_image->GetDirection();

    if(ITKIMAGE::ImageDimension > 2)
    {
        _image.set_orientation(
            {
                direction(0, 0),
                direction(0, 1),
                direction(0, 2),
                direction(1, 0),
                direction(1, 1),
                direction(1, 2),
                direction(2, 0),
                direction(2, 1),
                direction(2, 2)
            });
    }
    else
    {
        _image.set_orientation(
            {
                direction(0, 0),
                direction(0, 1),
                0,
                direction(1, 0),
                direction(1, 1),
                0,
                0,
                0,
                1
            });
    }

    const auto pixel_type = core::type::get<typename ITKIMAGE::PixelType>();
    const auto dump_lock  = _image.dump_lock();
    if(_buffer_manager_is_data_image)
    {
        SIGHT_ASSERT(
            "Sorry, this method requires that itkImage manages its buffer.",
            _itk_image->GetPixelContainer()->GetContainerManageMemory()
        );
        _image.set_buffer(
            static_cast<void*>(_itk_image->GetBufferPointer()),
            true,
            pixel_type,
            v_size,
            data::image::gray_scale,
            std::make_shared<core::memory::buffer_malloc_policy>()
        );
        /// itk image release its management buffer. dataImage must now deal memory
        _itk_image->GetPixelContainer()->SetContainerManageMemory(false);
    }
    else
    {
        _image.set_buffer(
            static_cast<void*>(_itk_image->GetBufferPointer()),
            false,
            pixel_type,
            v_size,
            data::image::gray_scale
        );
    }

    if(sight::data::helper::medical_image::check_image_validity(_image))
    {
        sight::data::helper::medical_image::check_image_slice_index(_image);
    }

    // Post Condition correct pixel_t
    SIGHT_ASSERT("Sorry, pixel type is not correct", _image.type() != core::type::NONE);
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
data::image::sptr move_from_itk(typename ITKIMAGE::Pointer _itk_image, bool _buffer_manager_is_data_image)
{
    auto data = std::make_shared<data::image>();
    io::itk::move_from_itk<ITKIMAGE>(_itk_image, *data, _buffer_manager_is_data_image);
    return data;
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
typename ITKIMAGE::Pointer move_to_itk(const data::image& _image)
{
    // Pre Condition
    SIGHT_ASSERT(
        "The itk image dimensions do not correspond to the input image",
        _image.num_dimensions() == ITKIMAGE::ImageDimension
    );

    const auto dump_lock = _image.dump_lock();

    typename ITKIMAGE::Pointer itk_image = ITKIMAGE::New();

    // update spacing information ; workaround due to GetSpacing const
    typename ITKIMAGE::SpacingType spacing = itk_image->GetSpacing();
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        spacing[d] = _image.spacing()[d];
    }

    itk_image->SetSpacing(spacing);

    // update origin information
    itk_image->SetOrigin(_image.origin().data());

    // Update direction information
    const auto& orientation = _image.orientation();
    auto direction          = itk_image->GetDirection();

    direction(0, 0) = orientation[0];
    direction(0, 1) = orientation[1];
    direction(1, 0) = orientation[3];
    direction(1, 1) = orientation[4];

    if(ITKIMAGE::ImageDimension > 2)
    {
        direction(0, 2) = orientation[2];
        direction(1, 2) = orientation[5];
        direction(2, 0) = orientation[6];
        direction(2, 1) = orientation[7];
        direction(2, 2) = orientation[8];
    }

    itk_image->SetDirection(direction);

    ::itk::ImageRegion<ITKIMAGE::ImageDimension> itk_region;

    std::uint64_t nb_pixels = 1;
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        // itkRegion.SetIndex( d,  static_cast<int>(imageData->getOrigin()[d]) );
        itk_region.SetSize(d, static_cast<std::uint64_t>(_image.size()[d]));
        nb_pixels *= static_cast<std::uint64_t>(itk_region.GetSize()[d]);
    }

    itk_image->SetRegions(itk_region);

    itk_image->GetPixelContainer()->SetImportPointer(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        static_cast<typename ITKIMAGE::PixelType*>(const_cast<void*>(_image.buffer())),
        nb_pixels,
        false
    );

    return itk_image;
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
typename ITKIMAGE::Pointer move_to_itk(data::image::csptr _data_image)
{
    SIGHT_ASSERT("Image is NULL", _data_image);
    return move_to_itk<ITKIMAGE>(*_data_image);
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
