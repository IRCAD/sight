/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/Bundle.hpp"

#include "fwRuntime/profile/Profile.hpp"
#include "fwRuntime/profile/Uninitializer.hpp"
#include "fwRuntime/profile/Initializer.hpp"

namespace fwRuntime
{

namespace profile
{

//------------------------------------------------------------------------------

Initializer::Initializer( const std::string & identifier )
: m_identifier( identifier )
{}

//------------------------------------------------------------------------------

void Initializer::apply()
{
    SPTR( Bundle )  bundle = Runtime::getDefault()->findBundle(m_identifier);
    OSLM_FATAL_IF("Unable to initialize bundle " << m_identifier << ". Not found.", bundle == 0);
    try
    {
        if (!bundle->isInitialized())
        {
            bundle->initialize();
            ::fwRuntime::profile::getCurrentProfile()->add( SPTR(Uninitializer) (new Uninitializer(m_identifier)));
        }
    }
    catch( const std::exception & e )
    {
        OSLM_FATAL("Unable to initialize bundle " << m_identifier << ". " << e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
