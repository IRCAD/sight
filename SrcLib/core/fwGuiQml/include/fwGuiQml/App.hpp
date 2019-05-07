/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "fwGuiQml/config.hpp"

#include <fwRuntime/profile/Profile.hpp>

#include <QGuiApplication>

namespace fwGuiQml
{

/**
 * @brief   Defines the Qml application.
 */
class FWGUIQML_CLASS_API App : public QGuiApplication
{
Q_OBJECT

public:
    FWGUIQML_API App(int& argc, char** argv);

public Q_SLOTS:
    void aboutToQuit();
    void onExit();

};

} // namespace fwGuiQml
