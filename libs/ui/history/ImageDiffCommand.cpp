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

#include "ui/history/ImageDiffCommand.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/type.hpp>

namespace sight::ui::history
{

//------------------------------------------------------------------------------

ImageDiffCommand::ImageDiffCommand(const data::image::sptr& _img, filter::image::image_diff _diff) :
    m_img(_img),
    m_modifiedSig(_img->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG)),
    m_diff(std::move(_diff))
{
    m_diff.shrink();
}

//------------------------------------------------------------------------------

std::size_t ImageDiffCommand::size() const
{
    return sizeof(*this) + m_diff.size();
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::redo()
{
    m_diff.applyDiff(m_img);

    m_modifiedSig->async_emit();

    return true;
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::undo()
{
    m_diff.revertDiff(m_img);

    m_modifiedSig->async_emit();

    return true;
}

//------------------------------------------------------------------------------

std::string ImageDiffCommand::getDescription() const
{
    return "image_diff";
}

} // namespace sight::ui::history
