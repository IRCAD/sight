/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/runtime/module_element.hpp"

#include "core/runtime/detail/module.hpp"

namespace sight::core::runtime
{

module_element::module_element() :
    m_module(detail::module::get_loading_module())
{
    // Post-condition
    SIGHT_ASSERT("Module '" << m_module.lock()->identifier() << "' not initialized", m_module.lock() != nullptr);
}

//------------------------------------------------------------------------------

module_element::module_element(std::shared_ptr<module> module) :
    m_module(module)
{
    // Post-condition
    SIGHT_ASSERT("Module '" << m_module.lock()->identifier() << "' not initialized", m_module.lock() != nullptr);
}

//------------------------------------------------------------------------------

std::shared_ptr<module> module_element::get_module() const
{
    return m_module.lock();
}

//------------------------------------------------------------------------------

bool module_element::enabled() const
{
    // Pre-condition.
    std::shared_ptr<detail::module> module = std::dynamic_pointer_cast<detail::module>(m_module.lock());
    SIGHT_ASSERT("module not initialized", module != nullptr);

    return module->enabled() && m_enable;
}

//------------------------------------------------------------------------------

void module_element::set_enable(const bool enable)
{
    m_enable = enable;
}

} // namespace sight::core::runtime
