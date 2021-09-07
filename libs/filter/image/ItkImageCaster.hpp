/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Image.hpp>

#include <io/itk/itk.hpp>

#include <itkCastImageFilter.h>

template<typename OUTPUT_PIXELTYPE>
struct ItkImageCaster
{
    using OutputImageType = ::itk::Image<OUTPUT_PIXELTYPE, 3>;

    struct Params
    {
        sight::data::Image::csptr i_img;
        typename OutputImageType::Pointer o_img;
    };

    //------------------------------------------------------------------------------

    template<typename INPUT_PIXELTYPE>
    void operator()(Params& p)
    {
        using InputImageType = ::itk::Image<INPUT_PIXELTYPE, 3>;

        // Convert to ITK.
        typename InputImageType::Pointer tmp = sight::io::itk::itkImageFactory<InputImageType>(p.i_img);

        // Cast to the desired pixel type.
        auto castFilter = ::itk::CastImageFilter<InputImageType, OutputImageType>::New();
        castFilter->SetInput(tmp);
        castFilter->Update();
        p.o_img = castFilter->GetOutput();
    }
};

//------------------------------------------------------------------------------

template<typename OUTPUT_PIXELTYPE>
typename ::itk::Image<OUTPUT_PIXELTYPE, 3>::Pointer castTo(const sight::data::Image::csptr& _img)
{
    using CasterType = ItkImageCaster<OUTPUT_PIXELTYPE>;

    typename CasterType::Params p;
    p.i_img = _img;

    const auto inType = _img->getType();

    sight::core::tools::Dispatcher<sight::core::tools::SupportedDispatcherTypes, CasterType>::invoke(inType, p);

    return p.o_img;
}
