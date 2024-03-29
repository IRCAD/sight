/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <QGuiApplication>

namespace sight::ui::qml
{

/**
 * @brief   Defines the Qml application.
 */
class UI_QML_CLASS_API app : public QGuiApplication
{
Q_OBJECT

public:

    UI_QML_API app(int& _argc, char** _argv);

    UI_QML_API bool notify(QObject* _receiver, QEvent* _e) override;

public Q_SLOTS:

    void aboutToQuit();
    static void on_exit();
};

} // namespace sight::ui::qml
