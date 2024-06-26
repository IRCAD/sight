/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <sight/ui/history/config.hpp>

#include "ui/history/command.hpp"

#include <data/image.hpp>

#include <filter/image/image.hpp>
#include <filter/image/image_diff.hpp>

namespace sight::ui::history
{

class SIGHT_UI_HISTORY_CLASS_API image_diff_command : public command
{
public:

    /// Constructor, uses an image and a change list for that image.
    SIGHT_UI_HISTORY_API image_diff_command(const data::image::sptr& _img, filter::image::image_diff _diff);

    /// The diff size.
    [[nodiscard]] SIGHT_UI_HISTORY_API std::size_t size() const override;

    /// Apply diff.
    SIGHT_UI_HISTORY_API bool redo() override;

    /// Revert diff.
    SIGHT_UI_HISTORY_API bool undo() override;

    /// Returns "image_diff".
    [[nodiscard]] SIGHT_UI_HISTORY_API std::string get_description() const override;

private:

    data::image::sptr m_img;

    data::image::buffer_modified_signal_t::sptr m_modified_sig;

    filter::image::image_diff m_diff;
};

} // namespace sight::ui::history
