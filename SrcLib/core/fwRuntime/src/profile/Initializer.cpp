/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    SLM_FATAL_IF("Unable to initialize bundle " + m_identifier + "_" + m_version.string() + ". Not found.",
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
        SLM_FATAL("Unable to initialize bundle " + m_identifier + "_" + m_version.string() + ". " + e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
