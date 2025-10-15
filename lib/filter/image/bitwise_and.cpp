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

#include "filter/image/bitwise_and.hpp"

#include <core/tools/dispatcher.hpp>

#include <io/itk/itk.hpp>

#include <itkAndImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

namespace sight::filter::image
{

struct and_image_filter_parameters
{
    const data::image& input_image;
    const data::image& mask;
    data::image& output_image;
};

//------------------------------------------------------------------------------

template<typename PIXELTYPE>
struct and_image_filter
{
    //------------------------------------------------------------------------------

    template<class MASK_PIXELTYPE>
    void operator()(and_image_filter_parameters& _params)
    {
        [[maybe_unused]] const unsigned int dimension = 3;
        SIGHT_ASSERT("Only image dimension 3 managed.", _params.input_image.num_dimensions() == dimension);

        using input_image_t  = typename itk::Image<PIXELTYPE, dimension>;
        using mask_image_t   = typename itk::Image<MASK_PIXELTYPE, dimension>;
        using output_image_t = typename itk::Image<PIXELTYPE, dimension>;

        auto itk_input_image = io::itk::move_to_itk<input_image_t>(_params.input_image);
        auto itk_mask_image  = io::itk::move_to_itk<mask_image_t>(_params.mask);
        typename output_image_t::Pointer itk_output_image;

        // We assume that the mask pixel type has a lower size in bits than the image pixel type
        // Cast mask pixel type to the image pixel type
        auto caster = itk::CastImageFilter<mask_image_t, input_image_t>::New();
        caster->SetInput(itk_mask_image);

        // Rescale the image so that the output range of the casted mask image is in the same range as the input image.
        auto rescaler = itk::RescaleIntensityImageFilter<input_image_t, input_image_t>::New();
        rescaler->SetInput(caster->GetOutput());
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(std::numeric_limits<PIXELTYPE>::max());
        rescaler->Update();

        typename input_image_t::Pointer itk_mask_image_casted = rescaler->GetOutput();

        auto filter = itk::AndImageFilter<input_image_t, input_image_t, output_image_t>::New();
        filter->SetInput1(itk_input_image);
        filter->SetInput2(itk_mask_image_casted);
        itk_output_image = filter->GetOutput();
        filter->Update();

        itk_output_image->GetSource()->Update();
        io::itk::move_from_itk<output_image_t>(itk_output_image, _params.output_image);
    }
};

//------------------------------------------------------------------------------

struct and_image_filter_caller
{
    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(and_image_filter_parameters& _params)
    {
        const auto mask_type = _params.mask.type();
        core::tools::dispatcher<core::tools::integer_types, and_image_filter<PIXELTYPE> >::invoke(mask_type, _params);
    }
};

//-----------------------------------------------------------------------------

void bitwise_and(
    const data::image& _in_image,
    const data::image& _mask_image,
    data::image& _out_image
)
{
    and_image_filter_parameters params {
        .input_image  = _in_image,
        .mask         = _mask_image,
        .output_image = _out_image
    };

    const auto type = _in_image.type();
    core::tools::dispatcher<core::tools::integer_types, and_image_filter_caller>::invoke(type, params);
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
