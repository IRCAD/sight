/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
