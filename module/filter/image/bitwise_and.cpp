/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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

#include <filter/image/bitwise_and.hpp>

namespace sight::module::filter::image
{

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

    sight::filter::image::bitwise_and(*image, *mask, *output_image);

    this->set_output(output_image, OUTPUTIMAGE_OUT);
    this->async_emit(signals::COMPUTED);
}

//-----------------------------------------------------------------------------

void bitwise_and::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image.
