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

#include "core/runtime/detail/profile/Starter.hpp"

#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/Runtime.hpp"
#include <core/base.hpp>

#include <iostream>
#include <sstream>

namespace sight::core::runtime
{

namespace detail
{

namespace profile
{

//------------------------------------------------------------------------------

Starter::Starter( const std::string& identifier, const Version& version) :
    m_identifier( identifier ),
    m_version( version )
{
}

//------------------------------------------------------------------------------

void Starter::apply()
{
    auto module = detail::Runtime::get().findEnabledModule(m_identifier, m_version);
    SLM_FATAL_IF("Unable to start module " + Module::getModuleStr(m_identifier, m_version) + ": not found.",
                 module == nullptr);
    try
    {
        if(!module->isStarted())
        {
            module->start();
        }
        else
        {
            SLM_WARN("module " + Module::getModuleStr(m_identifier, m_version) + " already started");
        }
    }
    catch( const std::exception& e )
    {
        SLM_FATAL("Unable to start module " + Module::getModuleStr(m_identifier, m_version) + ". " +
                  e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace detail

} // namespace profile

} // namespace sight::core::runtime
