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

#include "ui/qt/config.hpp"

#include <ui/__/dialog/logger_base.hpp>

#include <QDialog>
#include <QPointer>
#include <QTableWidget>

#include <vector>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   Defines a dialog showing log information.
 */
class UI_QT_CLASS_API_QT logger : public QDialog,
                                  public ui::dialog::logger_base
{
Q_OBJECT

public:

    SIGHT_DECLARE_CLASS(logger, ui::dialog::logger_base, ui::factory::make<logger>);

    /// Destructor
    UI_QT_API_QT ~logger() override = default;

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    UI_QT_API_QT void setTitle(const std::string& _title) override;

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    UI_QT_API_QT void setMessage(const std::string& _message) override;

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    UI_QT_API_QT void setLogger(const core::log::logger::sptr& _logger) override;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    UI_QT_API_QT bool show() override;

protected Q_SLOTS:

    /**
     * @brief Slot called when the user wants to display the logs
     * @param[in] state Checkbox state
     */
    void displayLogs(int _state);

private:

    /// Dialog title
    std::string m_title;

    /// Dialog message
    std::string m_message;

    /// Logger
    core::log::logger::sptr m_logger;

    /// Qt dialog
    QPointer<QDialog> m_dialog;

    /// Table widget used to display logs
    QPointer<QTableWidget> m_logTableWidget;
};

} // namespace sight::ui::qt::dialog
