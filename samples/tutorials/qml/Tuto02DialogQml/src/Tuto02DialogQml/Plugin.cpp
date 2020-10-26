/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
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

#include "Tuto02DialogQml/Plugin.hpp"

#include "Tuto02DialogQml/InputDialogs.hpp"
#include "Tuto02DialogQml/LocationDialogs.hpp"
#include "Tuto02DialogQml/LoggerDialogs.hpp"
#include "Tuto02DialogQml/MessageDialogs.hpp"
#include "Tuto02DialogQml/MultiSelectorDialogs.hpp"
#include "Tuto02DialogQml/ProgressDialogs.hpp"
#include "Tuto02DialogQml/PulseProgressDialogs.hpp"
#include "Tuto02DialogQml/SelectorDialogs.hpp"

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace Tuto02DialogQml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar(
    "::Tuto02DialogQml::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept
{
}

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType<SelectorDialogs>("Tuto02DialogQml", 1, 0, "DialoggSelectorDialogs");
    qmlRegisterType<InputDialogs>("Tuto02DialogQml", 1, 0, "DialoggInputDialogs");
    qmlRegisterType<LocationDialogs>("Tuto02DialogQml", 1, 0, "DialoggLocationDialogs");
    qmlRegisterType<LoggerDialogs>("Tuto02DialogQml", 1, 0, "DialoggLoggerDialogs");
    qmlRegisterType<MessageDialogs>("Tuto02DialogQml", 1, 0, "DialoggMessageDialogs");
    qmlRegisterType<ProgressDialogs>("Tuto02DialogQml", 1, 0, "DialoggProgressDialogs");
    qmlRegisterType<PulseProgressDialogs>("Tuto02DialogQml", 1, 0, "DialoggPulseProgressDialogs");
    qmlRegisterType<MultiSelectorDialogs>("Tuto02DialogQml", 1, 0, "DialoggMultiSelectorDialogs");

}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    const auto& path = ::fwRuntime::getModuleResourceFilePath("Tuto02DialogQml", "ui.qml");

    engine->loadMainComponent(path);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{

}

//------------------------------------------------------------------------------

} // namespace Tuto02DialogQml.
