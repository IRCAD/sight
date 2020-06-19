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

#include "fwGuiQml/App.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/Os.hpp>

#include <filesystem>
#include <locale.h>
#include <QCursor>

#include <iostream>
#include <sstream>

namespace fwGuiQml
{

//-----------------------------------------------------------------------------

App::App(int& argc, char** argv) :
    QGuiApplication(argc, argv)
{
    SLM_TRACE_FUNC();

    setlocale(LC_ALL, "C"); // needed for mfo save process
    QLocale::setDefault(QLocale::C); // on Linux we need that as well...

    std::string appName = "No name";

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    if (profile)
    {
        appName = profile->getName();
    }

    this->setApplicationName( QString::fromStdString(appName) );

    QObject::connect(this, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
}

//-----------------------------------------------------------------------------

void App::aboutToQuit()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void App::onExit()
{
    SLM_TRACE_FUNC();

    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QGuiApplication::restoreOverrideCursor();

    qGuiApp->sendPostedEvents();
    qGuiApp->exit(0);
}

//-----------------------------------------------------------------------------

} // namespace fwGuiQml
