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

#include "fwRuntime/profile/Stopper.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/Runtime.hpp"

#include <fwCore/base.hpp>

#include <iostream>
#include <sstream>

namespace fwRuntime
{

namespace profile
{

//------------------------------------------------------------------------------

Stopper::Stopper( const std::string& identifier, const Version& version) :
    m_identifier( identifier ),
    m_version( version )
{
}

//------------------------------------------------------------------------------

void Stopper::apply()
{
    std::shared_ptr< Bundle >  bundle = Runtime::getDefault()->findEnabledBundle(m_identifier, m_version);
    SLM_FATAL_IF("Unable to stop bundle " + Bundle::getBundleStr(m_identifier, m_version) + ". Not found.",
                 bundle == 0);
    try
    {
        SLM_INFO("Stopping bundle : " + Bundle::getBundleStr(m_identifier, m_version));
        bundle->stop();
    }
    catch( const std::exception& e )
    {
        SLM_ERROR("Unable to stop bundle " + Bundle::getBundleStr(m_identifier, m_version) + ". " + e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
