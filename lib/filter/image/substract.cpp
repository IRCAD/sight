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

//-----------------------------------------------------------------------------

void substract(const data::image& _image1, const data::image& _image2, data::image& _output)
{
    auto do_substract =
        []<class PIXEL_TYPE>(const data::image& _image1, const data::image& _image2, data::image& _output)
        {
            constexpr unsigned int dimension = 3;
            SIGHT_ASSERT("Only image dimension 3 managed.", _image1.num_dimensions() == dimension);

            using image_t = itk::Image<PIXEL_TYPE, dimension>;

            auto itk_image1 = io::itk::move_to_itk<image_t>(_image1);
            SIGHT_ASSERT("Unable to convert data::image to itkImage", itk_image1);

            auto itk_image2 = io::itk::move_to_itk<image_t>(_image2);
            SIGHT_ASSERT("Unable to convert data::image to itkImage", itk_image2);

            auto filter = itk::SubtractImageFilter<image_t, image_t, image_t>::New();
            SIGHT_ASSERT("Filter is null", filter);

            filter->SetInput1(itk_image1);
            filter->SetInput2(itk_image2);
            filter->Update();
            auto output = filter->GetOutput();
            SIGHT_ASSERT("Output is null", output->GetSource());
            io::itk::move_from_itk<image_t>(output, _output, true);
        };

    const auto type = _image1.type();
    using sight::core::tools::dispatcher;
    using sight::core::tools::integer_types;
    dispatcher<integer_types, decltype(do_substract)>::invoke(type, _image1, _image2, _output);
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
