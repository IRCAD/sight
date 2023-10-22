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

#include "ui/qt/app.hpp"

#include <core/runtime/exit_exception.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/tools/os.hpp>

#include <ui/__/dialog/message.hpp>

#include <boost/tokenizer.hpp>

#include <QFontDatabase>

#include <clocale>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

app::app(int& _argc, char** _argv, bool _gui_enabled) :
    QApplication(_argc, _argv, static_cast<int>(_gui_enabled))
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

    sight::ui::qt::app::setApplicationName(QString::fromStdString(app_name));

    QObject::connect(this, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));

    // Parse all font in rc/fonts folder.
    const auto fonts_folder = core::runtime::get_library_resource_file_path("sight::ui::qt/fonts");

    for(const auto& font : std::filesystem::recursive_directory_iterator {fonts_folder})
    {
        if(font.path().extension() == ".ttf")
        {
            QFontDatabase::addApplicationFont(QString::fromStdString(font.path().string()));
        }
    }
}

//-----------------------------------------------------------------------------

void app::aboutToQuit()
{
}

//-----------------------------------------------------------------------------

void app::onExit()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QApplication::restoreOverrideCursor();

    qApp->exit(0);
}

//------------------------------------------------------------------------------

bool app::notify(QObject* _receiver, QEvent* _e)
{
    try
    {
        return QApplication::notify(_receiver, _e);
    }
    catch(const core::runtime::exit_exception& e)
    {
        SIGHT_DEBUG("Exit exception caught. Exit code:" << e.what());
        qApp->exit(e.exit_code());
        return false;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt
