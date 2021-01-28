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
#include "ui/base/dialog/ILoggerDialog.hpp"

#include <core/base.hpp>
#include <core/log/Logger.hpp>

namespace sight::ui::base
{
namespace dialog
{
/**
 * @brief ILoggerDialog is used to display a dialog message and a set of logs
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation is fwQt libraries
 */
class UI_BASE_CLASS_API LoggerDialog : public ILoggerDialog
{

public:

    fwCoreClassMacro(LoggerDialog, ui::base::dialog::ILoggerDialog, new LoggerDialog)

    /**
     * Opens a default logger dialog with the specified title, message and logs.
     * @param[in] title Title of the dialog
     * @param[in] message Message of the dialog
     * @param[in] logger Logger of the dialog
     * @return Returns true or false whether the user has selected the OK or CANCEL button
     */
    UI_BASE_API static bool showLoggerDialog(const std::string& title, const std::string& message,
                                             const core::log::Logger::sptr& logger);

    /// Constructor that will instantiate the concrete implementation
    UI_BASE_API LoggerDialog();

    /// Destructor
    UI_BASE_API virtual ~LoggerDialog();

    /**
     * Constructor. Create a selector with the specified title, message and logs.
     * @param[in] title Title of the dialog
     * @param[in] message Message of the dialog
     * @param[in] logger Logger of the dialog
     */
    UI_BASE_API LoggerDialog(const std::string& title, const std::string& message,
                             const core::log::Logger::sptr& logger);

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    UI_BASE_API virtual void setTitle(const std::string& title) override;

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    UI_BASE_API virtual void setMessage(const std::string& message) override;

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    UI_BASE_API virtual void setLogger(const core::log::Logger::sptr& logger) override;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    UI_BASE_API virtual bool show() override;

protected:

    ui::base::dialog::ILoggerDialog::sptr m_implementation;

};

} // namespace dialog
} // namespace sight::ui::base
