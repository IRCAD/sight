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
#include "ui/base/dialog/IMultiSelectorDialog.hpp"

#include <core/base.hpp>

namespace sight::ui::base::dialog
{

/**
 * @brief   MultiSelectorDialog allows the choice of some element among several (_selections)
 *
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro
 * The specific implementation are in fwGuiWX and fwGuiQT libraries
 *
 */
class UI_BASE_CLASS_API MultiSelectorDialog : public IMultiSelectorDialog
{
public:

    SIGHT_DECLARE_CLASS(MultiSelectorDialog, ui::base::dialog::IMultiSelectorDialog, new MultiSelectorDialog);

    /// will instantiate the concrete implementation
    UI_BASE_API MultiSelectorDialog();

    /// Sets the selector title.
    UI_BASE_API void setTitle(std::string title) override;

    /// Set the string list that can be chosen by the selector.
    UI_BASE_API void setSelections(Selections _selections) override;

    /**
     * @brief Show the selector and return the selection.
     */
    UI_BASE_API Selections show() override;

    /// Set the message
    UI_BASE_API void setMessage(const std::string& msg) override;

protected:

    ui::base::dialog::IMultiSelectorDialog::sptr m_implementation;
};

} // namespace sight::ui::base::dialog
