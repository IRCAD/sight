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

#include "module/filter/image/over.hpp"

#include <filter/image/over.hpp>

#include <algorithm>

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

over::over() noexcept :
    filter(has_signals::signals())
{
}

//-----------------------------------------------------------------------------

void over::starting()
{
}

//-----------------------------------------------------------------------------

void over::stopping()
{
}

//-----------------------------------------------------------------------------

void over::updating()
{
    std::vector<data::image::csptr> images;
    std::vector<data::mt::locked_ptr<const data::image> > locked_images;
    images.reserve(m_images.size());
    locked_images.reserve(m_images.size());

    for(const auto& [index, image_ptr] : m_images)
    {
        SIGHT_NOT_USED(index);
        auto image = image_ptr->lock();
        SIGHT_THROW_IF("An input image is not set.", !image);
        images.push_back(image.get_shared());
        locked_images.push_back(std::move(image));
    }

    auto output = m_output.lock();
    SIGHT_THROW_IF("The output image is not set.", !output);

    if(std::ranges::any_of(
           images,
           [](const auto& _image)
        {
            return _image->num_elements() == 0;
        }))
    {
        output->shallow_copy(data::factory::make(output->get_classname()));
        output->async_emit(data::signals::MODIFIED);
        this->async_emit(signals::SUCCEEDED);
        return;
    }

    if(output->size() != images.front()->size())
    {
        output->resize(images.front()->size(), images.front()->type(), images.front()->pixel_format());
    }

    sight::filter::image::over(images, *output);
    output->async_emit(data::signals::MODIFIED);
    this->async_emit(signals::SUCCEEDED);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image
