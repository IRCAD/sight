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
#include "ui/__/dialog/progress_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic progress dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class UI_CLASS_API progress : public progress_base
{
public:

    SIGHT_DECLARE_CLASS(progress, ui::dialog::progress_base);

    /// will instantiate the concrete implementation
    UI_API progress(const std::string& _title = std::string(), const std::string& _msg = std::string());
    UI_API ~progress() override;

    ///set the title for the dialog
    UI_API void setTitle(const std::string& _title) override;

    ///set the message for the dialog
    UI_API void setMessage(const std::string& _message) override;

    /// action called by core::tools::progress_adviser
    UI_API void operator()(float _percent, std::string _msg) override;

    UI_API void setCancelCallback(cancel_callback_t _callback) override;

    UI_API void hideCancelButton() override;

protected:

    UI_API void cancelPressed() override;

    ui::dialog::progress_base::sptr m_implementation;
};

} // namespace sight::ui::dialog
