/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/profile/Uninitializer.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/Runtime.hpp"

#include <fwCore/base.hpp>

namespace fwRuntime
{

namespace profile
{

//------------------------------------------------------------------------------

Uninitializer::Uninitializer( const std::string& identifier, const Version& version ) :
    m_identifier( identifier ),
    m_version( version )
{
}

//------------------------------------------------------------------------------

void Uninitializer::apply()
{
    std::shared_ptr< Bundle >  bundle = Runtime::getDefault()->findEnabledBundle(m_identifier, m_version);
    SLM_FATAL_IF("Unable to uninitialize bundle " + Bundle::getBundleStr(m_identifier, m_version) + ". Not found.",
                 bundle == 0);
    try
    {
        bundle->uninitialize();
    }
    catch( const std::exception& e )
    {
        SLM_FATAL("Unable to uninitialize bundle " + Bundle::getBundleStr(m_identifier, m_version) + ". " + e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
