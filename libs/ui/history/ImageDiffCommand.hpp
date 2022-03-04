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

#pragma once

#include "ui/history/config.hpp"
#include "ui/history/ICommand.hpp"

#include <core/data/Image.hpp>

#include <filter/image/Image.hpp>
#include <filter/image/ImageDiff.hpp>

namespace sight::ui::history
{

class UI_HISTORY_CLASS_API ImageDiffCommand : public ICommand
{
public:

    /// Constructor, uses an image and a change list for that image.
    UI_HISTORY_API ImageDiffCommand(const data::Image::sptr& img, filter::image::ImageDiff diff);

    /// The diff size.
    UI_HISTORY_API virtual std::size_t getSize() const;

    /// Apply diff.
    UI_HISTORY_API virtual bool redo();

    /// Revert diff.
    UI_HISTORY_API virtual bool undo();

    /// Returns "ImageDiff".
    UI_HISTORY_API virtual const std::string getDescription() const;

private:

    data::Image::sptr m_img;

    data::Image::BufferModifiedSignalType::sptr m_modifiedSig;

    filter::image::ImageDiff m_diff;
};

} // namespace sight::ui::history
