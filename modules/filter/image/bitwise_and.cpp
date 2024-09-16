/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "bitwise_and.hpp"

#include <core/com/signal.hxx>
#include <core/tools/dispatcher.hpp>

#include <io/itk/itk.hpp>

#include <itkAndImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

namespace sight::module::filter::image
{

struct and_image_filter_parameters
{
    data::image::csptr input_image;
    data::image::csptr mask;
    data::image::sptr output_image;
};

//------------------------------------------------------------------------------

template<typename PIXELTYPE>
struct and_image_filter
{
    //------------------------------------------------------------------------------

    template<class MASK_PIXELTYPE>
    void operator()(and_image_filter_parameters& _params)
    {
        data::image::csptr input_image = _params.input_image;
        data::image::csptr mask        = _params.mask;
        data::image::sptr output_image = _params.output_image;

        [[maybe_unused]] const unsigned int dimension = 3;
        SIGHT_ASSERT("Only image dimension 3 managed.", input_image->num_dimensions() == dimension);

        using input_image_t  = typename itk::Image<PIXELTYPE, dimension>;
        using mask_image_t   = typename itk::Image<MASK_PIXELTYPE, dimension>;
        using output_image_t = typename itk::Image<PIXELTYPE, dimension>;

        typename input_image_t::Pointer itk_input_image = io::itk::move_to_itk<input_image_t>(input_image);
        typename mask_image_t::Pointer itk_mask_image   = io::itk::move_to_itk<mask_image_t>(mask);
        typename output_image_t::Pointer itk_output_image;

        // We assume that the mask pixel type has a lower size in bits than the image pixel type
        // Cast mask pixel type to the image pixel type
        using filter_t = itk::CastImageFilter<mask_image_t, input_image_t>;
        typename filter_t::Pointer caster = filter_t::New();
        caster->SetInput(itk_mask_image);

        // Rescale the image so that the output range of the casted mask image is in the same range as the input image.
        using rescale_t = itk::RescaleIntensityImageFilter<input_image_t, input_image_t>;
        typename rescale_t::Pointer rescaler = rescale_t::New();
        rescaler->SetInput(caster->GetOutput());
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(std::numeric_limits<PIXELTYPE>::max());
        rescaler->Update();

        typename input_image_t::Pointer itk_mask_image_casted = rescaler->GetOutput();

        using itk_filter_type_t = typename itk::AndImageFilter<input_image_t, input_image_t, output_image_t>;
        auto filter = itk_filter_type_t::New();
        filter->SetInput1(itk_input_image);
        filter->SetInput2(itk_mask_image_casted);
        itk_output_image = filter->GetOutput();
        filter->Update();

        itk_output_image->GetSource()->Update();
        io::itk::move_from_itk<output_image_t>(itk_output_image, output_image);
    }
};

//------------------------------------------------------------------------------

struct and_image_filter_caller
{
    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(and_image_filter_parameters& _params)
    {
        const auto mask_type = _params.mask->type();
        core::tools::dispatcher<core::tools::integer_types, and_image_filter<PIXELTYPE> >::invoke(mask_type, _params);
    }
};

//-----------------------------------------------------------------------------

bitwise_and::bitwise_and() :
    filter(m_signals)
{
}

//-----------------------------------------------------------------------------

void bitwise_and::configuring()
{
}

//-----------------------------------------------------------------------------

void bitwise_and::starting()
{
}

//-----------------------------------------------------------------------------

void bitwise_and::updating()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("image does not exist.", image);

    const auto mask = m_mask.lock();
    SIGHT_ASSERT("mask does not exist.", mask);

    data::image::sptr output_image = std::make_shared<data::image>();

    and_image_filter_parameters params;
    params.input_image  = image.get_shared();
    params.mask         = mask.get_shared();
    params.output_image = output_image;

    const auto type = image->type();
    core::tools::dispatcher<core::tools::integer_types, and_image_filter_caller>::invoke(type, params);

    this->set_output(output_image, OUTPUTIMAGE_OUT);

    this->signal<signals::computed_t>(signals::COMPUTED)->async_emit();
}

//-----------------------------------------------------------------------------

void bitwise_and::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image.
