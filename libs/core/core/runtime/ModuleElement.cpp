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

#include "core/runtime/ModuleElement.hpp"

#include "core/runtime/detail/Module.hpp"

namespace sight::core::runtime
{

ModuleElement::ModuleElement() :
    m_module(detail::Module::getLoadingModule()),
    m_enable(true)
{
    // Post-condition
    SIGHT_ASSERT("Module '" << m_module.lock()->getIdentifier() << "' not initialized", m_module.lock() != nullptr);
}

//------------------------------------------------------------------------------

ModuleElement::ModuleElement(std::shared_ptr<Module> module) :
    m_module(module),
    m_enable(true)
{
    // Post-condition
    SIGHT_ASSERT("Module '" << m_module.lock()->getIdentifier() << "' not initialized", m_module.lock() != nullptr);
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> ModuleElement::getModule() const
{
    return m_module.lock();
}

//------------------------------------------------------------------------------

bool ModuleElement::isEnabled() const
{
    // Pre-condition.
    std::shared_ptr<detail::Module> module = std::dynamic_pointer_cast<detail::Module>(m_module.lock());
    SIGHT_ASSERT("module not initialized", module != nullptr);

    return module->isEnabled() && m_enable;
}

//------------------------------------------------------------------------------

void ModuleElement::setEnable(const bool enable)
{
    m_enable = enable;
}

} // namespace sight::core::runtime
