/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/dialog/pulse_progress_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic pulse progress dialog for UI.
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class SIGHT_UI_CLASS_API pulse_progress : public pulse_progress_base
{
public:

    SIGHT_DECLARE_CLASS(pulse_progress, ui::dialog::pulse_progress_base);

    //------------------------------------------------------------------------------

    static sptr make(const std::string& _title, const ui::dialog::pulse_progress_base::Stuff& _stuff)
    {
        return std::make_shared<pulse_progress>(_title, _stuff);
    }

    /// will instantiate the concrete implementation
    SIGHT_UI_API pulse_progress(
        const std::string& _title,
        Stuff _stuff,
        const std::string& _msg                                         = std::string(),
        ui::dialog::pulse_progress_base::MilliSecond _frequence_refresh = 100,
        bool _cancellable                                               = true
    );

    ///set the title for the dialog
    SIGHT_UI_API void set_title(const std::string& _title) override;

    ///set the message for the dialog
    SIGHT_UI_API void set_message(const std::string& _message) override;

    ///allow to cancel the dialog
    SIGHT_UI_API void set_cancellable(bool _cancellable) override;

    ///show the pulse progress dialog
    SIGHT_UI_API void show() override;

protected:

    ui::dialog::pulse_progress_base::sptr m_implementation;
};

} // namespace sight::ui::dialog
