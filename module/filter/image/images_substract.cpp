/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "images_substract.hpp"

#include <core/com/signal.hxx>

#include <filter/image/substract.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

images_substract::images_substract() :
    filter(has_signals::signals())
{
}

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
    const bool is_same_type = (image1->type() == image2->type() && image1->type() == requested_type);

    if(is_same_type)
    {
        // test if the both images have the same size.
        const bool is_same_size = (image1->size() == image2->size());
        if(is_same_size)
        {
            sight::filter::image::substract(*image1, *image2, *image_result);
            image_result->async_emit(data::object::MODIFIED_SIG);
        }
        else
        {
            sight::ui::dialog::message::show(
                "Warning",
                "Both images must have the same size.",
                sight::ui::dialog::message::warning
            );
        }
    }
    else
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Both Images must have signed short as type.",
            sight::ui::dialog::message::warning
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
