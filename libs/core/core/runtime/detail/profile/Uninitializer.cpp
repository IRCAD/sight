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

#include "core/runtime/detail/profile/Uninitializer.hpp"

#include "core/runtime/detail/Module.hpp"
#include "core/runtime/detail/Runtime.hpp"

#include <core/base.hpp>

namespace sight::core::runtime
{

namespace detail
{

namespace profile
{

//------------------------------------------------------------------------------

Uninitializer::Uninitializer( const std::string& identifier ) :
    m_identifier( identifier )
{
}

//------------------------------------------------------------------------------

void Uninitializer::apply()
{
    auto module = detail::Runtime::get().findEnabledModule(m_identifier);
    SLM_FATAL_IF("Unable to start module " + m_identifier + ": not found.",
                 module == nullptr);
    try
    {
        module->uninitialize();
    }
    catch( const std::exception& e )
    {
        SLM_FATAL("Unable to uninitialize module " + m_identifier + " : " + e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace detail

} // namespace sight::core::runtime
