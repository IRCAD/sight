/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "service/SConfigController.hpp"

#include "service/macros.hpp"

namespace sight::service
{

//------------------------------------------------------------------------------

SConfigController::SConfigController() noexcept :
    m_configLauncher(std::make_unique<service::helper::ConfigLauncher>())
{
}

//------------------------------------------------------------------------------

SConfigController::~SConfigController() noexcept =
    default;

//------------------------------------------------------------------------------

void SConfigController::starting()
{
    m_configLauncher->startConfig(this->getSptr());
}

//------------------------------------------------------------------------------

void SConfigController::stopping()
{
    m_configLauncher->stopConfig();
}

//------------------------------------------------------------------------------

void SConfigController::configuring()
{
    m_configLauncher->parseConfig(this->getConfiguration(), this->getSptr());
}

//------------------------------------------------------------------------------

void SConfigController::updating()
{
}

//------------------------------------------------------------------------------

void SConfigController::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::service
