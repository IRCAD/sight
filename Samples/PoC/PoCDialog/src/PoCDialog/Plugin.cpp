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

#include "PoCDialog/AppManager.hpp"

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
    qmlRegisterType<AppManager>("PoCDialog", 1, 0, "AppManager");
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    m_input = std::shared_ptr<InputDialogs>(new InputDialogs);

    m_location = std::shared_ptr<LocationDialogs>(new LocationDialogs);

    m_logger = std::shared_ptr<LoggerDialogs>(new LoggerDialogs);

    m_message = std::shared_ptr<MessageDialogs>(new MessageDialogs);

    m_multiSelector = std::shared_ptr<MultiSelectorDialogs>(new MultiSelectorDialogs);

    m_selector = std::shared_ptr<SelectorDialogs>(new SelectorDialogs);

    m_progress = std::shared_ptr<ProgressDialogs>(new ProgressDialogs);

    m_pulseProgress = std::shared_ptr<PulseProgressDialogs>(new PulseProgressDialogs);

    auto path = ::fwRuntime::getBundleResourceFilePath("PoCDialog-0.1/ui.qml");

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
