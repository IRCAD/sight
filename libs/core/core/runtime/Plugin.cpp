/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/runtime/Plugin.hpp"

#include "core/runtime/ConfigurationElement.hpp"

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

std::shared_ptr<Module> Plugin::getModule() const noexcept
{
    return m_module.lock();
}

//------------------------------------------------------------------------------

void Plugin::setInitializationData(const ConfigurationElement::sptr) noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::setModule(std::shared_ptr<Module> module) noexcept
{
    m_module = module;
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime
