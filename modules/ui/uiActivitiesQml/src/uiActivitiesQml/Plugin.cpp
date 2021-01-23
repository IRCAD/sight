/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "uiActivitiesQml/Plugin.hpp"

#include "uiActivitiesQml/ActivityLauncherManager.hpp"
#include "uiActivitiesQml/SActivitySequencer.hpp"
#include "uiActivitiesQml/SActivityView.hpp"

#include <core/runtime/operations.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwQml/QmlEngine.hpp>

#include <QIcon>

namespace uiActivitiesQml
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiActivitiesQml::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType< SActivityView >("uiActivitiesQml", 1, 0, "SActivityView");
    qmlRegisterType< SActivitySequencer >("uiActivitiesQml", 1, 0, "SActivitySequencer");
    qmlRegisterType< ActivityLauncherManager >("uiActivitiesQml", 1, 0, "ActivityLauncherManager");
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    auto path = core::runtime::getModuleResourcePath("uiActivitiesQml");

    engine->importModulePath(path);
    QStringList iconPath = QIcon::themeSearchPaths();
    iconPath.append(QString::fromStdString(path.string()));
    QIcon::setThemeSearchPaths(iconPath);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace uiActivitiesQml
