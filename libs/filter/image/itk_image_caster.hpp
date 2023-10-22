/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <core/tools/dispatcher.hpp>

#include <data/image.hpp>

#include <io/itk/itk.hpp>

#include <itkCastImageFilter.h>

template<typename OUTPUT_PIXELTYPE>
struct itk_image_caster
{
    using output_image_t = itk::Image<OUTPUT_PIXELTYPE, 3>;

    struct Params
    {
        sight::data::image::csptr i_img;
        typename output_image_t::Pointer o_img;
    };

    //------------------------------------------------------------------------------

    template<typename INPUT_PIXELTYPE>
    void operator()(Params& _p)
    {
        using input_image_t = itk::Image<INPUT_PIXELTYPE, 3>;

        // Convert to ITK.
        typename input_image_t::Pointer tmp = sight::io::itk::move_to_itk<input_image_t>(_p.i_img);

        // Cast to the desired pixel type.
        auto cast_filter = itk::CastImageFilter<input_image_t, output_image_t>::New();
        cast_filter->SetInput(tmp);
        cast_filter->Update();
        _p.o_img = cast_filter->GetOutput();
    }
};

//------------------------------------------------------------------------------

template<typename OUTPUT_PIXELTYPE>
typename itk::Image<OUTPUT_PIXELTYPE, 3>::Pointer cast_to(const sight::data::image::csptr& _img)
{
    using caster_t = itk_image_caster<OUTPUT_PIXELTYPE>;

    typename caster_t::Params p;
    p.i_img = _img;

    const auto in_type = _img->getType();

    sight::core::tools::dispatcher<sight::core::tools::supported_dispatcher_types, caster_t>::invoke(in_type, p);

    return p.o_img;
}
