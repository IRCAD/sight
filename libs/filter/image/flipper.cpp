/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "filter/image/flipper.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/medical_image.hpp>

#include <io/itk/itk.hpp>

#include <itkFlipImageFilter.h>

namespace sight::filter::image
{

struct Parameters
{
    data::image::csptr i_image;
    std::array<bool, 3> i_flipAxes {};
    data::image::sptr o_image;
};

//------------------------------------------------------------------------------

template<typename pixel_t, int dimension>
struct Flipping
{
    //------------------------------------------------------------------------------

    void operator()(Parameters& _params)
    {
        using image_t = typename itk::Image<pixel_t, dimension>;
        const typename image_t::Pointer itk_image = io::itk::move_to_itk<image_t>(_params.i_image);

        typename itk::FlipImageFilter<image_t>::Pointer flip_filter =
            itk::FlipImageFilter<image_t>::New();

        flip_filter->SetInput(itk_image);
        typename itk::FlipImageFilter<image_t>::FlipAxesArrayType axes;
        for(std::size_t i = 0 ; i < axes.Size() && i < _params.i_flipAxes.size() ; i++)
        {
            axes[i] = _params.i_flipAxes[i];
        }

        flip_filter->SetFlipAxes(axes);
        flip_filter->Update();

        typename image_t::Pointer output_image = flip_filter->GetOutput();
        io::itk::move_from_itk(output_image, _params.o_image);
    }
};

struct FlippingDimensionExtractor
{
    //------------------------------------------------------------------------------

    template<class pixel_t>
    void operator()(Parameters& _params)
    {
        const data::image::Size size = _params.i_image->size();
        switch(size.size())
        {
            case 1:
                Flipping<pixel_t, 1> d1;
                d1(_params);
                break;

            case 2:
                Flipping<pixel_t, 2> d2;
                d2(_params);
                break;

            case 3:
                Flipping<pixel_t, 3> d3;
                d3(_params);
                break;

            default:
                SIGHT_ERROR(
                    "Flipping cannot be performed due to incompatible image size ("
                    + std::to_string(size.size()) + ")."
                );
                // In this case, we just deep copy the input image in the output
                _params.o_image->deep_copy(_params.i_image);
                break;
        }
    }
};

//-----------------------------------------------------------------------------

void flipper::flip(
    const data::image::csptr& _in_image,
    const data::image::sptr& _out_image,
    const std::array<bool, 3>& _in_flip_axes
)
{
    // If the image is valid, process it, otherwise copy it in the output image
    if(data::helper::medical_image::check_image_validity(_in_image))
    {
        Parameters params;
        params.i_image    = _in_image;
        params.i_flipAxes = _in_flip_axes;
        params.o_image    = _out_image;

        const core::type type = _in_image->getType();
        core::tools::dispatcher<core::tools::supported_dispatcher_types, FlippingDimensionExtractor>::invoke(
            type,
            params
        );
    }
    else
    {
        _out_image->deep_copy(_in_image);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
