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

#include "module/filter/image/threshold.hpp"

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <filter/image/threshold.hpp>

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

threshold::threshold() noexcept :
    filter(has_signals::signals())
{
}

//-----------------------------------------------------------------------------

void threshold::starting()
{
}

//-----------------------------------------------------------------------------

void threshold::stopping()
{
}

//-----------------------------------------------------------------------------

void threshold::updating()
{
    // retrieve the input object
    auto input  = m_source.lock();
    auto output = m_target.lock();

    SIGHT_THROW_IF("Invalid input image", !input);
    SIGHT_THROW_IF("Invalid output image", !output);

    if(input->num_elements() == 0)
    {
        output->shallow_copy(data::factory::make(output->get_classname()));
        output->async_emit(data::signals::MODIFIED);
        this->async_emit(signals::SUCCEEDED);
        return;
    }

    if(output->size() != input->size())
    {
        output->resize(input->size(), input->type(), input->pixel_format());
    }

    sight::filter::image::threshold(*input, *m_lower_threshold, *m_upper_threshold, *output, *m_binary);
    output->async_emit(data::signals::MODIFIED);
    this->async_emit(signals::SUCCEEDED);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image
