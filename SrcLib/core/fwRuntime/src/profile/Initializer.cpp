/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwRuntime/profile/Initializer.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/profile/Profile.hpp"
#include "fwRuntime/profile/Uninitializer.hpp"
#include "fwRuntime/Runtime.hpp"

#include <fwCore/base.hpp>

namespace fwRuntime
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
    SPTR( Bundle )  bundle = Runtime::getDefault()->findEnabledBundle(m_identifier, m_version);
    SLM_FATAL_IF("Unable to initialize bundle " + Bundle::getBundleStr(m_identifier, m_version) + ". Not found.",
                 bundle == 0);
    try
    {
        if (!bundle->isInitialized())
        {
            bundle->initialize();
            ::fwRuntime::profile::getCurrentProfile()->add( SPTR(Uninitializer) (new Uninitializer(m_identifier,
                                                                                                   m_version)));
        }
    }
    catch( const std::exception& e )
    {
        SLM_FATAL("Unable to initialize bundle " + Bundle::getBundleStr(m_identifier, m_version) + ". " + e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
