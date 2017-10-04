/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/profile/Starter.hpp"

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

Starter::Starter( const std::string& identifier, const Version& version) :
    m_identifier( identifier ),
    m_version( version )
{
}

//------------------------------------------------------------------------------

void Starter::apply()
{
    std::shared_ptr< Bundle >  bundle = Runtime::getDefault()->findEnabledBundle(m_identifier, m_version);
    SLM_FATAL_IF(
        "Unable to start bundle " + m_identifier + Bundle::s_VERSION_DELIMITER  + m_version.string() + ". Not found.",
        bundle == 0);
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
        SLM_FATAL(
            "Unable to start bundle " + m_identifier + Bundle::s_VERSION_DELIMITER  + m_version.string() + ". " +
            e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
