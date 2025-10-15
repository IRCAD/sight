/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "filter/image/substract.hpp"

#include <core/tools/dispatcher.hpp>

#include <io/itk/itk.hpp>

#include <itkSubtractImageFilter.h>

namespace sight::filter::image
{

struct substract_parameters
{
    const data::image& image1;
    const data::image& image2;
    data::image& output_image;
};

//------------------------------------------------------------------------------

struct substract_image_filter
{
    //------------------------------------------------------------------------------

    template<typename PIXELTYPE>
    void operator()(substract_parameters& _params)
    {
        [[maybe_unused]] const unsigned int dimension = 3;
        SIGHT_ASSERT("Only image dimension 3 managed.", _params.image1.num_dimensions() == dimension);

        using image_t = itk::Image<PIXELTYPE, dimension>;

        auto itk_image1 = io::itk::move_to_itk<image_t>(_params.image1);
        SIGHT_ASSERT("Unable to convert data::image to itkImage", itk_image1);

        auto itk_image2 = io::itk::move_to_itk<image_t>(_params.image2);
        SIGHT_ASSERT("Unable to convert data::image to itkImage", itk_image2);

        typename image_t::Pointer output;

        //Create filter
        auto filter = itk::SubtractImageFilter<image_t, image_t, image_t>::New();
        assert(filter);

        filter->SetInput1(itk_image1);
        filter->SetInput2(itk_image2);
        filter->Update();
        output = filter->GetOutput();
        assert(output->GetSource());
        io::itk::move_from_itk<image_t>(output, _params.output_image, true);
    }
};

//-----------------------------------------------------------------------------

void substract(const data::image& _image1, const data::image& _image2, data::image& _output)
{
    substract_parameters params {
        .image1       = _image1,
        .image2       = _image2,
        .output_image = _output
    };

    const auto type = _image1.type();
    core::tools::dispatcher<core::tools::integer_types, substract_image_filter>::invoke(type, params);
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
