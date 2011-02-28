/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/Bundle.hpp"

#include "fwRuntime/profile/Uninitializer.hpp"


namespace fwRuntime
{

namespace profile
{

//------------------------------------------------------------------------------

Uninitializer::Uninitializer( const std::string & identifier )
: m_identifier( identifier )
{}

//------------------------------------------------------------------------------

void Uninitializer::apply()
{
    ::boost::shared_ptr< Bundle >  bundle = Runtime::getDefault()->findBundle(m_identifier);
    OSLM_FATAL_IF("Unable to uninitialize bundle " << m_identifier << ". Not found.", bundle == 0);
    try
    {
        bundle->uninitialize();
    }
    catch( const std::exception & e )
    {
        OSLM_FATAL("Unable to uninitialize bundle " << m_identifier << ". " << e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
