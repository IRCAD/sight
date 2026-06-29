/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "folder_selector.hpp"

#include <core/location/single_folder.hpp>

#include <ui/__/dialog/location.hpp>

namespace sight::module::ui::io
{

//------------------------------------------------------------------------------

folder_selector::folder_selector() noexcept
{
    new_signal<signals::folder_selected_t>(signals::FOLDER_SELECTED);
}

//------------------------------------------------------------------------------

folder_selector::~folder_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void folder_selector::configuring()
{
    const auto& config = this->get_config();
    m_dialog_title = config.get("dialogTitle", "Select a folder");
}

//------------------------------------------------------------------------------

void folder_selector::starting()
{
}

//------------------------------------------------------------------------------

void folder_selector::updating()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_dialog_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(sight::ui::dialog::location::read);
    dialog_file.set_type(sight::ui::dialog::location::folder);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        this->async_emit(signals::FOLDER_SELECTED, result->get_folder());
    }
}

//------------------------------------------------------------------------------

void folder_selector::stopping()
{
}

//------------------------------------------------------------------------------
//

} // namespace sight::module::ui::io
