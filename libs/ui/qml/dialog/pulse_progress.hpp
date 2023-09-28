/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qml/config.hpp"

#include <ui/__/dialog/pulse_progress_base.hpp>

#include <QObject>

#include <string>

namespace sight::ui::qml::dialog
{

/**
 * @brief   This class allows us to show a pulse progress bar.
 */
class UI_QML_CLASS_API pulse_progress : public QObject,
                                        public ui::dialog::pulse_progress_base
{
Q_OBJECT
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)

public:

    SIGHT_DECLARE_CLASS(
        pulse_progress,
        ui::dialog::pulse_progress_base,
        ui::factory::make<pulse_progress>
    )

    UI_QML_API ~pulse_progress() override = default;

    /// override
    UI_QML_API void setTitle(const std::string& title) override;

    /// override
    UI_QML_API void setMessage(const std::string& message) override;

    UI_QML_API void show() override;

Q_SIGNALS:

    /// notify the qml of property change
    void messageChanged();

private:

    /// title of the progress dialog
    QString m_title;
    /// message of the progress dialog
    QString m_message;

    /// event filter to filter shortcut event
    bool eventFilter(QObject* watched, QEvent* event) override;
};

} // namespace sight::ui::qml::dialog
