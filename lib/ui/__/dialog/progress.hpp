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

#include "ui/__/dialog/progress_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic progress dialog for UI.
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class SIGHT_UI_CLASS_API progress : public progress_base
{
public:

    SIGHT_DECLARE_CLASS(progress, ui::dialog::progress_base);

    /// will instantiate the concrete implementation
    SIGHT_UI_API progress(const std::string& _title = std::string(), const std::string& _msg = std::string());
    SIGHT_UI_API ~progress() override;

    ///set the title for the dialog
    SIGHT_UI_API void set_title(const std::string& _title) override;

    ///set the message for the dialog
    SIGHT_UI_API void set_message(const std::string& _message) override;

    /// action called by core::tools::progress_adviser
    SIGHT_UI_API void operator()(float _percent, std::string _msg) override;

    SIGHT_UI_API void set_cancel_callback(cancel_callback_t _callback) override;

    SIGHT_UI_API void hide_cancel_button() override;

protected:

    SIGHT_UI_API void cancel_pressed() override;

    ui::dialog::progress_base::sptr m_implementation;
};

} // namespace sight::ui::dialog
