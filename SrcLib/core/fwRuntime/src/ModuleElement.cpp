/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwRuntime/ModuleElement.hpp"

#include "fwRuntime/impl/Module.hpp"

namespace fwRuntime
{

ModuleElement::ModuleElement() :
    m_module( impl::Module::getLoadingModule() ),
    m_enable(true)
{
    // Post-condition
    SLM_ASSERT("Module '" << m_module.lock()->getIdentifier() << "' not initialized", m_module.lock() != nullptr );
}

//------------------------------------------------------------------------------

ModuleElement::ModuleElement( std::shared_ptr< Module > module ) :
    m_module( module ),
    m_enable(true)
{
    // Post-condition
    SLM_ASSERT("Module '" << m_module.lock()->getIdentifier() << "' not initialized", m_module.lock() != nullptr );
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> ModuleElement::getBundle() const
{
    FW_DEPRECATED_MSG("getBundle", "22.0");
    return this->getModule();
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
    std::shared_ptr< impl::Module > module = std::dynamic_pointer_cast< impl::Module >(m_module.lock());
    SLM_ASSERT("module not initialized", module != nullptr );

    return module->isEnabled() && m_enable;
}

//------------------------------------------------------------------------------

void ModuleElement::setEnable(const bool enable)
{
    m_enable = enable;
}

} // namespace fwRuntime
