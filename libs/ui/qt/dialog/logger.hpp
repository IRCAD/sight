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

#include <sight/ui/qt/config.hpp>

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
class SIGHT_UI_QT_CLASS_API_QT logger : public QDialog,
                                        public ui::dialog::logger_base
{
Q_OBJECT

public:

    SIGHT_DECLARE_CLASS(logger, ui::dialog::logger_base, ui::factory::make<logger>);

    /// Destructor
    SIGHT_UI_QT_API_QT ~logger() override = default;

    /**
     * @brief Set the dialog title.
     * @param[in] _title Dialog title
     */
    SIGHT_UI_QT_API_QT void set_title(const std::string& _title) override;

    /**
     * @brief Set the dialog message.
     * @param[in] _message Dialog message
     */
    SIGHT_UI_QT_API_QT void set_message(const std::string& _message) override;

    /**
     * @brief Set the dialog logger.
     * @param[in] _logger Dialog logger
     */
    SIGHT_UI_QT_API_QT void set_logger(const core::log::logger::sptr& _logger) override;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    SIGHT_UI_QT_API_QT bool show() override;

protected Q_SLOTS:

    /**
     * @brief Slot called when the user wants to display the logs
     * @param[in] _state Checkbox state
     */
    void display_logs(int _state);

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
    QPointer<QTableWidget> m_log_table_widget;
};

} // namespace sight::ui::qt::dialog
