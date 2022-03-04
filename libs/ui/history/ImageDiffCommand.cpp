/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/tools/Type.hpp>

namespace sight::ui::history
{

//------------------------------------------------------------------------------

ImageDiffCommand::ImageDiffCommand(const data::Image::sptr& img, filter::image::ImageDiff diff) :
    m_img(img),
    m_modifiedSig(img->signal<data::Image::BufferModifiedSignalType>(data::Image::s_BUFFER_MODIFIED_SIG)),
    m_diff(diff)
{
    m_diff.shrink();
}

//------------------------------------------------------------------------------

std::size_t ImageDiffCommand::getSize() const
{
    return sizeof(*this) + m_diff.getSize();
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::redo()
{
    m_diff.applyDiff(m_img);

    m_modifiedSig->asyncEmit();

    return true;
}

//------------------------------------------------------------------------------

bool ImageDiffCommand::undo()
{
    m_diff.revertDiff(m_img);

    m_modifiedSig->asyncEmit();

    return true;
}

//------------------------------------------------------------------------------

const std::string ImageDiffCommand::getDescription() const
{
    return "ImageDiff";
}

} // namespace sight::ui::history
