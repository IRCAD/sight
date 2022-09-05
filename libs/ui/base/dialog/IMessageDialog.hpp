/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "ui/base/GuiBaseObject.hpp"

#include <string>

namespace sight::ui::base::dialog
{

/**
 * @brief   Defines the generic message box for IHM.
 */
class UI_BASE_CLASS_API IMessageDialog : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(IMessageDialog, ui::base::GuiBaseObject);

    /// Icon type
    typedef enum
    {
        CRITICAL,
        WARNING,
        INFO,
        QUESTION,
        NONE
    } Icons;

    /// Buttons type
    typedef enum
    {
        NOBUTTON = 0, // invalid

        OK     = 1 << 1,
        YES    = 1 << 2,
        NO     = 1 << 3,
        CANCEL = 1 << 4,
        RETRY  = 1 << 5,
        YES_NO = YES | NO
    } Buttons;

    /// Constructor. Do nothing.
    UI_BASE_API IMessageDialog();
    /// Destructor. Do nothing.
    UI_BASE_API ~IMessageDialog() override;

    typedef std::string FactoryRegistryKeyType;
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the title of the message box
    UI_BASE_API virtual void setTitle(const std::string& title) = 0;

    /// Set the message
    UI_BASE_API virtual void setMessage(const std::string& msg) = 0;

    /// Set the icon (CRITICAL, WARNING, INFO or QUESTION)
    UI_BASE_API virtual void setIcon(Icons icon) = 0;

    /// Add a button (OK, YES_NO, YES, NO, CANCEL, RETRY)
    UI_BASE_API virtual void addButton(Buttons button) = 0;

    /// Set the default button
    UI_BASE_API virtual void setDefaultButton(Buttons button) = 0;

    /// Add a custom button to this dialog
    UI_BASE_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) = 0;

    /// Show the message box and return the clicked button.
    UI_BASE_API virtual Buttons show() = 0;
};

} // namespace sight::ui::base::dialog
