/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/qt/config.hpp>

#include "ui/qt/container/widget.hpp"

#include <ui/__/dialog/progress_base.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QString>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace sight::ui::qt::dialog
{

/**
 * @brief   Allows to select an acquisition in a patient database.
 *
 * @note Do not inherit from fwProgressDialog but embed a fwProgressDialog because this former is not copyable.
 * @todo progress is not commented.
 */
class SIGHT_UI_QT_CLASS_API_QT progress : public QObject,
                                          public ui::dialog::progress_base
{
Q_OBJECT

public:

    SIGHT_DECLARE_CLASS(progress, ui::dialog::progress_base, ui::factory::make<progress>);

    SIGHT_UI_QT_API_QT progress(
        const std::string& _title   = "Progression",
        const std::string& _message = std::string(86, ' ')
    );

    SIGHT_UI_QT_API_QT ~progress() override;

    SIGHT_UI_QT_API_QT void operator()(float _percent, std::string _msg) override;

    /// override
    SIGHT_UI_QT_API_QT void set_title(const std::string& _title) override;

    /// override
    SIGHT_UI_QT_API_QT void set_message(const std::string& _message) override;

    SIGHT_UI_QT_API_QT void hide_cancel_button() override;

protected Q_SLOTS:

    void cancel_pressed() override;

protected:

    QString m_title;

    QPointer<QProgressDialog> m_dialog;
    QPointer<QProgressBar> m_progressbar;
    QPointer<QPushButton> m_cancel_button;
    QPointer<QMainWindow> m_main_window;
};

} // namespace sight::ui::qt::dialog
