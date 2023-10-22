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

#include <io/itk/itk.hpp>

#include <core/com/signal.hxx>

#include <core/spy_log.hpp>

#include <ui/__/dialog/message.hpp>

// Services tools
#include <service/macros.hpp>

#include "images_substract.hpp"

#include <itkSubtractImageFilter.h>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

void images_substract::configuring()
{
}

//------------------------------------------------------------------------------

void images_substract::starting()
{
}

//------------------------------------------------------------------------------

void images_substract::stopping()
{
}

//------------------------------------------------------------------------------

void images_substract::updating()
{
    core::type requested_type = core::type::INT16;

    const auto image1 = m_image1.lock();
    const auto image2 = m_image2.lock();
    auto image_result = m_result.lock();

    // Test if the both images have the same type and it is signed short.
    const bool is_same_type = (image1->getType() == image2->getType() && image1->getType() == requested_type);

    if(is_same_type)
    {
        // test if the both images have the same size.
        const bool is_same_size = (image1->size() == image2->size());
        if(is_same_size)
        {
            using image_t = itk::Image<std::int16_t, 3>;

            image_t::Pointer itk_image1 = io::itk::move_to_itk<image_t>(image1.get_shared());
            SIGHT_ASSERT("Unable to convert data::image to itkImage", itk_image1);

            image_t::Pointer itk_image2 = io::itk::move_to_itk<image_t>(image2.get_shared());
            SIGHT_ASSERT("Unable to convert data::image to itkImage", itk_image2);

            image_t::Pointer output;

            //Create filter
            using subtract_image_filter_t = itk::SubtractImageFilter<image_t, image_t, image_t>;
            subtract_image_filter_t::Pointer filter;
            filter = subtract_image_filter_t::New();
            assert(filter);

            filter->SetInput1(itk_image1);
            filter->SetInput2(itk_image2);
            filter->Update();
            output = filter->GetOutput();
            assert(output->GetSource());
            io::itk::move_from_itk<image_t>(output, image_result.get_shared(), true);

            auto sig = image_result->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();
        }
        else
        {
            sight::ui::dialog::message::show(
                "Warning",
                "Both images must have the same size.",
                sight::ui::dialog::message::WARNING
            );
        }
    }
    else
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Both Images must have signed short as type.",
            sight::ui::dialog::message::WARNING
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
