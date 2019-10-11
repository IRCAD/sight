/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwRuntime/impl/profile/Starter.hpp"

#include "fwRuntime/impl/Bundle.hpp"
#include "fwRuntime/impl/Runtime.hpp"

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

namespace fwRuntime
{

namespace impl
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
    auto bundle = impl::Runtime::get().findEnabledBundle(m_identifier, m_version);
    SLM_FATAL_IF("Unable to start bundle " + Bundle::getBundleStr(m_identifier, m_version) + ": not found.",
                 bundle == nullptr);
    try
    {
        if(!bundle->isStarted())
        {
            bundle->start();
        }
        else
        {
            SLM_WARN("bundle " + Bundle::getBundleStr(m_identifier, m_version) + " already started");
        }
    }
    catch( const std::exception& e )
    {
        SLM_FATAL("Unable to start bundle " + Bundle::getBundleStr(m_identifier, m_version) + ". " +
                  e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace impl

} // namespace profile

} // namespace fwRuntime
