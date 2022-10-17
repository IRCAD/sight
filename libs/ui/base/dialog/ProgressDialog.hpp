/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/dialog/IProgressDialog.hpp"

#include <core/base.hpp>

namespace sight::ui::base::dialog
{

/**
 * @brief   Defines the generic progress dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class UI_BASE_CLASS_API ProgressDialog : public IProgressDialog
{
public:

    SIGHT_DECLARE_CLASS(ProgressDialog, ui::base::dialog::IProgressDialog, new ProgressDialog);

    /// will instantiate the concrete implementation
    UI_BASE_API ProgressDialog(const std::string& title = std::string(), const std::string& msg = std::string());
    UI_BASE_API ~ProgressDialog() override;

    ///set the title for the dialog
    UI_BASE_API void setTitle(const std::string& title) override;

    ///set the message for the dialog
    UI_BASE_API void setMessage(const std::string& message) override;

    /// action called by core::tools::ProgressAdviser
    UI_BASE_API void operator()(float percent, std::string msg) override;

    UI_BASE_API void setCancelCallback(CancelCallbackType callback) override;

    UI_BASE_API void hideCancelButton() override;

protected:

    UI_BASE_API void cancelPressed() override;

    ui::base::dialog::IProgressDialog::sptr m_implementation;
};

} // namespace sight::ui::base::dialog
