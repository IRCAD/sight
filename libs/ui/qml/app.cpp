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

#include "app.hpp"

#include <core/runtime/exit_exception.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/tools/os.hpp>

#include <ui/__/dialog/message.hpp>

#include <QCursor>

#include <clocale>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace sight::ui::qml
{

//-----------------------------------------------------------------------------

app::app(int& _argc, char** _argv) :
    QGuiApplication(_argc, _argv)
{
    if(setlocale(LC_ALL, "C") == nullptr) // needed for mfo save process
    {
        perror("setlocale");
    }

    QLocale::setDefault(QLocale::C); // on Linux we need that as well...

    std::string app_name = "No name";

    core::runtime::profile::sptr profile = core::runtime::get_current_profile();

    if(profile)
    {
        app_name = profile->name();
    }

    sight::ui::qml::app::setApplicationName(QString::fromStdString(app_name));

    QObject::connect(this, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
}

//-----------------------------------------------------------------------------

void app::aboutToQuit()
{
}

//-----------------------------------------------------------------------------

void app::on_exit()
{
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QGuiApplication::restoreOverrideCursor();

    qGuiApp->sendPostedEvents();
    qGuiApp->exit(0);
}

//-----------------------------------------------------------------------------

bool app::notify(QObject* _receiver, QEvent* _e)
{
    try
    {
        return QGuiApplication::notify(_receiver, _e);
    }
    catch(const core::runtime::exit_exception& e)
    {
        SIGHT_DEBUG("Exit exception caught. Exit code:" << e.what());
        qGuiApp->exit(e.exit_code());
        return false;
    }
}

} // namespace sight::ui::qml
