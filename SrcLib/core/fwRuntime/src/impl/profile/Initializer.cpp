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

#include "fwRuntime/impl/profile/Initializer.hpp"

#include "fwRuntime/impl/Module.hpp"
#include "fwRuntime/impl/profile/Profile.hpp"
#include "fwRuntime/impl/profile/Uninitializer.hpp"
#include "fwRuntime/impl/Runtime.hpp"

#include <fwCore/base.hpp>

namespace fwRuntime
{

namespace impl
{

namespace profile
{

//------------------------------------------------------------------------------

Initializer::Initializer( const std::string& identifier, const Version& version) :
    m_identifier( identifier ),
    m_version( version )
{
}

//------------------------------------------------------------------------------

void Initializer::apply()
{
    auto module = impl::Runtime::get().findEnabledModule(m_identifier, m_version);
    SLM_FATAL_IF("Unable to initialize module " + Module::getModuleStr(m_identifier, m_version) + ". Not found.",
                 module == nullptr);
    try
    {
        if (!module->isInitialized())
        {
            module->initialize();
            getCurrentProfile()->add( SPTR(Uninitializer) (new Uninitializer(m_identifier, m_version)));
        }
    }
    catch( const std::exception& e )
    {
        SLM_FATAL("Unable to initialize module " + Module::getModuleStr(m_identifier, m_version) + ". " + e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace impl

} // namespace fwRuntime
