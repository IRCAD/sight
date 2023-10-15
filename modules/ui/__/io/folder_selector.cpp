/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/location/single_folder.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>

namespace sight::module::ui::io
{

const core::com::signals::key_t folder_selector::FOLDER_SELECTED_SIG = "folderSelected";

//------------------------------------------------------------------------------

folder_selector::folder_selector() noexcept
{
    new_signal<FolderSelectedSignalType>(FOLDER_SELECTED_SIG);
}

//------------------------------------------------------------------------------

folder_selector::~folder_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void folder_selector::configuring()
{
    const auto& config = this->get_config();
    m_dialogTitle = config.get("dialogTitle", "Select a folder");
}

//------------------------------------------------------------------------------

void folder_selector::starting()
{
}

//------------------------------------------------------------------------------

void folder_selector::updating()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_dialogTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(sight::ui::dialog::location::READ);
    dialogFile.setType(sight::ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());
    if(result)
    {
        auto sig = this->signal<FolderSelectedSignalType>(FOLDER_SELECTED_SIG);
        sig->async_emit(result->get_folder());
    }
}

//------------------------------------------------------------------------------

void folder_selector::stopping()
{
}

//------------------------------------------------------------------------------
//

} // namespace sight::module::ui::io
