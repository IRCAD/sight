/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/qt/App.hpp"

#include <core/runtime/ExitException.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/tools/Os.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <boost/tokenizer.hpp>

#include <QFontDatabase>

#include <clocale>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

App::App(int& argc, char** argv, bool guiEnabled) :
    QApplication(argc, argv, static_cast<int>(guiEnabled))
{
    if(setlocale(LC_ALL, "C") == nullptr) // needed for mfo save process
    {
        perror("setlocale");
    }

    QLocale::setDefault(QLocale::C); // on Linux we need that as well...

    std::string appName = "No name";

    core::runtime::Profile::sptr profile = core::runtime::getCurrentProfile();

    if(profile)
    {
        appName = profile->getName();
    }

    sight::ui::qt::App::setApplicationName(QString::fromStdString(appName));

    QObject::connect(this, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));

    // Parse all font in rc/fonts folder.
    const auto fonts_folder = core::runtime::getLibraryResourceFilePath("sight::ui::qt/fonts");

    for(const auto& font : std::filesystem::recursive_directory_iterator {fonts_folder})
    {
        if(font.path().extension() == ".ttf")
        {
            QFontDatabase::addApplicationFont(QString::fromStdString(font.path().string()));
        }
    }
}

//-----------------------------------------------------------------------------

void App::aboutToQuit()
{
}

//-----------------------------------------------------------------------------

void App::onExit()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::restoreOverrideCursor();

    qApp->exit(0);
}

//------------------------------------------------------------------------------

bool App::notify(QObject* receiver, QEvent* e)
{
    try
    {
        return QApplication::notify(receiver, e);
    }
    catch(const core::runtime::ExitException& e)
    {
        SIGHT_DEBUG("Exit exception caught. Exit code:" << e.what());
        qApp->exit(e.exitCode());
        return false;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt
