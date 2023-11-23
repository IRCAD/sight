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
    data::image::sptr _data_image,
    bool _buffer_manager_is_data_image
)
{
    SIGHT_ASSERT("_dataImage not instanced", _data_image);

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

    _data_image->set_origin(v_origin);
    _data_image->set_spacing(v_spacing);

    const auto pixel_type = core::type::get<typename ITKIMAGE::PixelType>();
    const auto dump_lock  = _data_image->dump_lock();
    if(_buffer_manager_is_data_image)
    {
        SIGHT_ASSERT(
            "Sorry, this method requires that itkImage manages its buffer.",
            _itk_image->GetPixelContainer()->GetContainerManageMemory()
        );
        _data_image->set_buffer(
            static_cast<void*>(_itk_image->GetBufferPointer()),
            true,
            pixel_type,
            v_size,
            data::image::gray_scale,
            std::make_shared<core::memory::buffer_new_policy>()
        );
        /// itk image release its management buffer. dataImage must now deal memory
        _itk_image->GetPixelContainer()->SetContainerManageMemory(false);
    }
    else
    {
        _data_image->set_buffer(
            static_cast<void*>(_itk_image->GetBufferPointer()),
            false,
            pixel_type,
            v_size,
            data::image::gray_scale
        );
    }

    if(sight::data::helper::medical_image::check_image_validity(_data_image))
    {
        sight::data::helper::medical_image::check_image_slice_index(_data_image);
    }

    // Post Condition correct pixel_t
    SIGHT_ASSERT("Sorry, pixel type is not correct", _data_image->type() != core::type::NONE);
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
data::image::sptr move_from_itk(typename ITKIMAGE::Pointer _itk_image, bool _buffer_manager_is_data_image)
{
    data::image::sptr data = std::make_shared<data::image>();
    io::itk::move_from_itk<ITKIMAGE>(_itk_image, data, _buffer_manager_is_data_image);
    return data;
}

//------------------------------------------------------------------------------

template<class ITKIMAGE_PTR>
void move_from_itk(ITKIMAGE_PTR _itk_image, data::image::sptr _data_image)
{
    move_from_itk<typename ITKIMAGE_PTR::ObjectType>(_itk_image, _data_image);
}

//------------------------------------------------------------------------------

template<class ITKIMAGE>
typename ITKIMAGE::Pointer move_to_itk(data::image::csptr _image_data)
{
    // Pre Condition
    SIGHT_ASSERT(
        "The itk image dimensions do not correspond to the input image",
        _image_data->num_dimensions() == ITKIMAGE::ImageDimension
    );

    const auto dump_lock = _image_data->dump_lock();

    typename ITKIMAGE::Pointer itk_image = ITKIMAGE::New();

    // update spacing information ; workaround due to GetSpacing const
    typename ITKIMAGE::SpacingType spacing = itk_image->GetSpacing();
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        spacing[d] = _image_data->spacing()[d];
    }

    itk_image->SetSpacing(spacing);

    // update origin information
    itk_image->SetOrigin(_image_data->origin().data());

    ::itk::ImageRegion<ITKIMAGE::ImageDimension> itk_region;

    std::uint64_t nb_pixels = 1;
    for(std::uint8_t d = 0 ; d < ITKIMAGE::ImageDimension ; ++d)
    {
        // itkRegion.SetIndex( d,  static_cast<int>(imageData->getOrigin()[d]) );
        itk_region.SetSize(d, static_cast<std::uint64_t>(_image_data->size()[d]));
        nb_pixels *= static_cast<std::uint64_t>(itk_region.GetSize()[d]);
    }

    itk_image->SetRegions(itk_region);

    itk_image->GetPixelContainer()->SetImportPointer(
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        static_cast<typename ITKIMAGE::PixelType*>(const_cast<void*>(_image_data->buffer())),
        nb_pixels,
        false
    );

    return itk_image;
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk
