/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "filter/image/cast.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/medical_image.hpp>

#include <filter/image/detail/itk_image_caster.hpp>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------
data::image::sptr cast_to(
    const data::image& _in_image,
    sight::core::type _out_type
)
{
    const auto do_cast_to =
        []<class OUTPUT_PIXELTYPE>(const data::image& _in_image, data::image::sptr& _out_image)
        {
            using output_image_t = itk::Image<OUTPUT_PIXELTYPE, 3>;
            //------------------------------------------------------------------------------

            const auto do_call_cast_to =
                []<class INPUT_PIXELTYPE>(const data::image& _in_image, data::image::sptr& _out_image)
                {
                    using input_image_t = itk::Image<INPUT_PIXELTYPE, 3>;

                    // Convert to ITK.
                    auto tmp = sight::io::itk::move_to_itk<input_image_t>(_in_image);

                    // Cast to the desired pixel type.
                    auto cast_filter = itk::CastImageFilter<input_image_t, output_image_t>::New();
                    cast_filter->SetInput(tmp);
                    cast_filter->Update();

                    io::itk::move_from_itk<output_image_t>(cast_filter->GetOutput(), *_out_image);
                };

            const auto in_type = _in_image.type();

            sight::core::tools::dispatcher<sight::core::tools::intrinsic_types, decltype(do_call_cast_to)>::invoke(
                in_type,
                _in_image,
                _out_image
            );

            return _out_image;
        };
    auto output_image = std::make_shared<data::image>();
    if(data::helper::medical_image::check_image_validity(_in_image))
    {
        core::tools::dispatcher<core::tools::intrinsic_types, decltype(do_cast_to)>::invoke(
            _out_type,
            _in_image,
            output_image
        );
    }

    return output_image;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
