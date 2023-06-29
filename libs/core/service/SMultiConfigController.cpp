/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "service/SMultiConfigController.hpp"

#include "service/macros.hpp"

#include <core/com/Slots.hxx>

namespace sight::service
{

//------------------------------------------------------------------------------

SMultiConfigController::SMultiConfigController() noexcept :
    m_configLauncher(std::make_unique<service::helper::ConfigLauncher>())
{
    newSlot(slots::s_SET_CONFIG_SLOT, &SMultiConfigController::setConfig, this);
}

//------------------------------------------------------------------------------

SMultiConfigController::~SMultiConfigController() noexcept =
    default;

//------------------------------------------------------------------------------

void SMultiConfigController::starting()
{
}

//------------------------------------------------------------------------------

void SMultiConfigController::stopping()
{
    m_configLauncher->stopConfig();
}

//------------------------------------------------------------------------------

void SMultiConfigController::configuring()
{
    m_configLauncher->parseConfig(this->getConfiguration(), this->getSptr());
    const auto& appConfig = this->getConfiguration().get_child("appConfig");
    m_key = appConfig.get<std::string>("<xmlattr>.key", m_key);
}

//------------------------------------------------------------------------------

void SMultiConfigController::updating()
{
    if(m_configLauncher->configIsRunning())
    {
        m_configLauncher->stopConfig();
    }

    m_configLauncher->startConfig(this->getSptr());
}

//------------------------------------------------------------------------------
void SMultiConfigController::setConfig(sight::ui::base::parameter_t val, std::string key)
{
    if(key == m_key && std::holds_alternative<std::string>(val))
    {
        m_configLauncher->setConfig(std::get<std::string>(val));
        this->update();
    }
}

//------------------------------------------------------------------------------

void SMultiConfigController::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::service
