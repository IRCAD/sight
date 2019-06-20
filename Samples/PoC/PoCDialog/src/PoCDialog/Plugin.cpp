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

#include "PoCDialog/Plugin.hpp"

#include "PoCDialog/InputDialogs.hpp"
#include "PoCDialog/LocationDialogs.hpp"
#include "PoCDialog/LoggerDialogs.hpp"
#include "PoCDialog/MessageDialogs.hpp"
#include "PoCDialog/MultiSelectorDialogs.hpp"
#include "PoCDialog/ProgressDialogs.hpp"
#include "PoCDialog/PulseProgressDialogs.hpp"
#include "PoCDialog/SelectorDialogs.hpp"

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace PoCDialog
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar(
    "::PoCDialog::Plugin");

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
    qmlRegisterType<SelectorDialogs>("PoCDialog", 1, 0, "PocDialogSelectorDialogs");
    qmlRegisterType<InputDialogs>("PoCDialog", 1, 0, "PocDialogInputDialogs");
    qmlRegisterType<LocationDialogs>("PoCDialog", 1, 0, "PocDialogLocationDialogs");
    qmlRegisterType<LoggerDialogs>("PoCDialog", 1, 0, "PocDialogLoggerDialogs");
    qmlRegisterType<MessageDialogs>("PoCDialog", 1, 0, "PocDialogMessageDialogs");
    qmlRegisterType<ProgressDialogs>("PoCDialog", 1, 0, "PocDialogProgressDialogs");
    qmlRegisterType<PulseProgressDialogs>("PoCDialog", 1, 0, "PocDialogPulseProgressDialogs");
    qmlRegisterType<MultiSelectorDialogs>("PoCDialog", 1, 0, "PocDialogMultiSelectorDialogs");
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    auto path = ::fwRuntime::getBundleResourceFilePath("PoCDialog", "ui.qml");

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

} // namespace PoCDialog
