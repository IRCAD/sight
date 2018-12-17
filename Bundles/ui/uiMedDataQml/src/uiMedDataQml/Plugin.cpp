/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "uiMedDataQml/Plugin.hpp"

#include "uiMedDataQml/OrganListModel.hpp"
#include "uiMedDataQml/SModelSeriesList.hpp"

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace uiMedDataQml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiMedDataQml::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType<SModelSeriesList>("uiMedDataQml", 1, 0, "SModelSeriesList");
    qmlRegisterType<OrganListModel>("uiMedDataQml", 1, 0, "OrganListModel");
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    auto path = ::fwRuntime::getBundleResourcePath("uiMedDataQml");

    engine->importModulePath(path);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace uiMedDataQml
