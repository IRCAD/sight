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

#include "ui/__/dialog/logger_base.hpp"

#include <core/base.hpp>
#include <core/log/logger.hpp>

namespace sight::ui::dialog
{

/**
 * @brief logger_base is used to display a dialog message and a set of logs
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation is fwQt libraries
 */
class SIGHT_UI_CLASS_API logger : public logger_base
{
public:

    SIGHT_DECLARE_CLASS(logger, ui::dialog::logger_base);

    /**
     * Opens a default logger dialog with the specified title, message and logs.
     * @param[in] _title Title of the dialog
     * @param[in] _message Message of the dialog
     * @param[in] _logger Logger of the dialog
     * @return Returns true or false whether the user has selected the OK or CANCEL button
     */
    SIGHT_UI_API static bool show_logger_dialog(
        const std::string& _title,
        const std::string& _message,
        const core::log::logger::sptr& _logger
    );

    /// Constructor that will instantiate the concrete implementation
    SIGHT_UI_API logger();

    /// Destructor
    SIGHT_UI_API ~logger() override = default;

    /**
     * Constructor. Create a selector with the specified title, message and logs.
     * @param[in] _title Title of the dialog
     * @param[in] _message Message of the dialog
     * @param[in] _logger Logger of the dialog
     */
    SIGHT_UI_API logger(
        const std::string& _title,
        const std::string& _message,
        const core::log::logger::sptr& _logger
    );

    /**
     * @brief Set the dialog title.
     * @param[in] _title Dialog title
     */
    SIGHT_UI_API void set_title(const std::string& _title) override;

    /**
     * @brief Set the dialog message.
     * @param[in] _message Dialog message
     */
    SIGHT_UI_API void set_message(const std::string& _message) override;

    /**
     * @brief Set the dialog logger.
     * @param[in] _logger Dialog logger
     */
    SIGHT_UI_API void set_logger(const core::log::logger::sptr& _logger) override;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    SIGHT_UI_API bool show() override;

protected:

    ui::dialog::logger_base::sptr m_implementation;
};

} // namespace sight::ui::dialog
