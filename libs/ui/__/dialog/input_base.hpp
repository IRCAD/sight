/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/object.hpp"

#include <string>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic input dialog box for IHM.
 */
class UI_CLASS_API input_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(input_base, ui::object);

    enum class EchoMode : uint8_t
    {
        NORMAL       = 0,
        NOECHO       = 1,
        PASSWORD     = 2,
        ECHO_ON_EDIT = 3,
        DEFAULT      = NORMAL
    };

    /// Constructor. Do nothing.
    UI_API input_base();
    /// Destructor. Do nothing.
    UI_API ~input_base() override;

    typedef std::string FactoryRegistryKeyType;
    UI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the title of the input dialog
    UI_API virtual void setTitle(const std::string& title) = 0;

    /// Set the input text in the input field
    UI_API virtual void setInput(const std::string& text) = 0;

    /// Get the input text in the input field
    UI_API virtual std::pair<std::string, bool> getInput() = 0;

    /// Set the message
    UI_API virtual void setMessage(const std::string& msg) = 0;

    /// Sets the echo mode used to display input field content
    UI_API virtual void setEchoMode(EchoMode echoMode) = 0;
};

} // namespace sight::ui::dialog
