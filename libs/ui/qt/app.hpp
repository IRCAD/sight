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

#include <QApplication>

namespace sight::ui::qt
{

/**
 * @brief   Defines the Qt application.
 */
class SIGHT_UI_QT_CLASS_API_QT app : public QApplication
{
Q_OBJECT

public:

    SIGHT_UI_QT_API_QT app(int& _argc, char** _argv, bool _gui_enabled);

    SIGHT_UI_QT_API_QT bool notify(QObject* _receiver, QEvent* _e) override;

public Q_SLOTS:

    void aboutToQuit();
    static void on_exit();
};

} // namespace sight::ui::qt
