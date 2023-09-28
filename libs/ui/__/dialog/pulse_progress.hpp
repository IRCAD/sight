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

#pragma once

#include "ui/__/config.hpp"
#include "ui/__/dialog/pulse_progress_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic pulse progress dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class UI_CLASS_API pulse_progress : public pulse_progress_base
{
public:

    SIGHT_DECLARE_CLASS(pulse_progress, ui::dialog::pulse_progress_base);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& title, const ui::dialog::pulse_progress_base::Stuff& stuff)
    {
        return std::make_shared<pulse_progress>(title, stuff);
    }

    /// will instantiate the concrete implementation
    UI_API pulse_progress(
        const std::string& title,
        Stuff stuff,
        const std::string& msg                                        = std::string(),
        ui::dialog::pulse_progress_base::MilliSecond frequenceRefresh = 100
    );

    ///set the title for the dialog
    UI_API void setTitle(const std::string& title) override;

    ///set the message for the dialog
    UI_API void setMessage(const std::string& message) override;

    ///show the pulse progress dialog
    UI_API void show() override;

protected:

    ui::dialog::pulse_progress_base::sptr m_implementation;
};

} // namespace sight::ui::dialog
