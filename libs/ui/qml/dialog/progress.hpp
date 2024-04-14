/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <sight/ui/qml/config.hpp>

#include <ui/__/dialog/progress_base.hpp>

#include <QObject>
#include <QPointer>
#include <QString>

#include <string>

namespace sight::ui::qml::dialog
{

/**
 * @brief   This class allows us to select an acquisition in a patient data base.
 */
class SIGHT_UI_QML_CLASS_API progress : public QObject,
                                        public ui::dialog::progress_base
{
Q_OBJECT
Q_PROPERTY(bool visible MEMBER m_visible)
Q_PROPERTY(QString title MEMBER m_title NOTIFY title_changed)
Q_PROPERTY(bool has_callback MEMBER m_has_callback NOTIFY has_callback_changed)

public:

    SIGHT_DECLARE_CLASS(progress, ui::dialog::progress_base, ui::factory::make<progress>);

    SIGHT_UI_QML_API progress(
        const std::string& _title   = "Progression",
        const std::string& _message = std::string(86, ' ')
    );

    SIGHT_UI_QML_API ~progress() override;

    /// the operator to set the progress of the percentage
    SIGHT_UI_QML_API void operator()(float _percent, std::string _msg) override;

    /// override
    SIGHT_UI_QML_API void set_title(const std::string& _title) override;

    /// override
    SIGHT_UI_QML_API void set_message(const std::string& _message) override;

    SIGHT_UI_QML_API void hide_cancel_button() override;

Q_SIGNALS:

    void title_changed();
    void has_callback_changed();

protected Q_SLOTS:

    void cancel_pressed() override;

private:

    /// the progress dialog to delete it during the destruction of the class
    QObject* m_dialog {nullptr};
    /// the window of the progress dialog to delete it during the destruction of the class
    QObject* m_window {nullptr};
    /// visible state of the progress dialog
    bool m_visible {false};
    /// title string
    QString m_title;
    /// boolean to enable Cancel button
    bool m_has_callback {true};
};

} // namespace sight::ui::qml::dialog
