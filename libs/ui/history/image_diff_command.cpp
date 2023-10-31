/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "ui/history/image_diff_command.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/type.hpp>

namespace sight::ui::history
{

//------------------------------------------------------------------------------

image_diff_command::image_diff_command(const data::image::sptr& _img, filter::image::image_diff _diff) :
    m_img(_img),
    m_modified_sig(_img->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG)),
    m_diff(std::move(_diff))
{
    m_diff.shrink();
}

//------------------------------------------------------------------------------

std::size_t image_diff_command::size() const
{
    return sizeof(*this) + m_diff.size();
}

//------------------------------------------------------------------------------

bool image_diff_command::redo()
{
    m_diff.apply_diff(m_img);

    m_modified_sig->async_emit();

    return true;
}

//------------------------------------------------------------------------------

bool image_diff_command::undo()
{
    m_diff.revert_diff(m_img);

    m_modified_sig->async_emit();

    return true;
}

//------------------------------------------------------------------------------

std::string image_diff_command::get_description() const
{
    return "image_diff";
}

} // namespace sight::ui::history
