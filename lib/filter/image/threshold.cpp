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

#include "threshold.hpp"

#include <core/tools/dispatcher.hpp>

#include <io/itk/itk.hpp>

#include <itkBinaryThresholdImageFilter.h>
#include <itkThresholdImageFilter.h>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

void threshold(
    const data::image& _input_image,
    std::int64_t _lower_threshold,
    std::int64_t _upper_threshold,
    data::image& _output_image,
    bool _binary
)
{
    SIGHT_THROW_IF("Images with more than 1 component are not supported.", _input_image.num_components() != 1);

    auto apply_threshold =
        []<class PIXEL_TYPE>(
            const data::image& _input_image,
            std::int64_t _lower_threshold,
            std::int64_t _upper_threshold,
            data::image& _output_image,
            bool _binary)
        {
            [[maybe_unused]] constexpr unsigned int dimension = 3;
            SIGHT_ASSERT("Only image dimension 3 is managed.", _input_image.num_dimensions() == dimension);

            using image_t = itk::Image<PIXEL_TYPE, dimension>;
            auto itk_input = io::itk::move_to_itk<image_t>(_input_image);

            SIGHT_ASSERT(
                "Lower threshold must be less than or equal to upper threshold.",
                _lower_threshold <= _upper_threshold
            );
            SIGHT_ASSERT(
                "Upper threshold must be less than or equal to the maximum value of the pixel type.",
                _upper_threshold <= static_cast<std::int64_t>(std::numeric_limits<PIXEL_TYPE>::max())
            );
            SIGHT_ASSERT(
                "Lower threshold must be greater than or equal to the minimum value of the pixel type.",
                _lower_threshold >= static_cast<std::int64_t>(std::numeric_limits<PIXEL_TYPE>::min())
            );

            typename image_t::Pointer itk_output;
            if(_binary)
            {
                auto threshold_filter = itk::BinaryThresholdImageFilter<image_t, image_t>::New();
                threshold_filter->SetInput(itk_input);
                threshold_filter->SetLowerThreshold(static_cast<PIXEL_TYPE>(_lower_threshold));
                threshold_filter->SetUpperThreshold(static_cast<PIXEL_TYPE>(_upper_threshold));
                threshold_filter->SetInsideValue(static_cast<PIXEL_TYPE>(std::numeric_limits<PIXEL_TYPE>::max()));
                threshold_filter->SetOutsideValue(static_cast<PIXEL_TYPE>(0));
                threshold_filter->Update();
                itk_output = threshold_filter->GetOutput();
            }
            else
            {
                auto threshold_filter = itk::ThresholdImageFilter<image_t>::New();
                threshold_filter->SetInput(itk_input);
                threshold_filter->ThresholdOutside(
                    static_cast<PIXEL_TYPE>(_lower_threshold),
                    static_cast<PIXEL_TYPE>(_upper_threshold)
                );
                threshold_filter->SetOutsideValue(static_cast<PIXEL_TYPE>(0));
                threshold_filter->Update();
                itk_output = threshold_filter->GetOutput();
            }

            io::itk::move_from_itk<image_t>(itk_output, _output_image);
        };

    core::tools::dispatcher<core::tools::integer_types, decltype(apply_threshold)>::invoke(
        _input_image.type(),
        _input_image,
        _lower_threshold,
        _upper_threshold,
        _output_image,
        _binary
    );
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
