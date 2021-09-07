/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "ui/base/dialog/IPulseProgressDialog.hpp"

#include <core/base.hpp>

namespace sight::ui::base
{

namespace dialog
{

/**
 * @brief   Defines the generic pulse progress dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 */
class UI_BASE_CLASS_API PulseProgressDialog : public IPulseProgressDialog
{
public:

    SIGHT_DECLARE_CLASS(PulseProgressDialog, ui::base::dialog::IPulseProgressDialog);

    //------------------------------------------------------------------------------

    static sptr New(const std::string& title, const ui::base::dialog::IPulseProgressDialog::Stuff& stuff)
    {
        return std::make_shared<PulseProgressDialog>(title, stuff);
    }

    /// will instantiate the concrete implementation
    UI_BASE_API PulseProgressDialog(
        const std::string& title,
        Stuff stuff,
        const std::string& msg                                               = std::string(),
        ui::base::dialog::IPulseProgressDialog::MilliSecond frequenceRefresh = 100
    );

    ///set the title for the dialog
    UI_BASE_API void setTitle(const std::string& title) override;

    ///set the message for the dialog
    UI_BASE_API void setMessage(const std::string& message) override;

    ///show the pulse progress dialog
    UI_BASE_API void show() override;

protected:

    ui::base::dialog::IPulseProgressDialog::sptr m_implementation;
};

} //namespace dialog

} // namespace sight::ui::base
