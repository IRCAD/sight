/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <iostream>
#include <sstream>

#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/Bundle.hpp"

#include <fwCore/base.hpp>

#include "fwRuntime/profile/Stopper.hpp"

namespace fwRuntime
{

namespace profile
{

//------------------------------------------------------------------------------

Stopper::Stopper( ::boost::shared_ptr< Starter > starter )
: Starter( starter->m_identifier )
{}

//------------------------------------------------------------------------------

void Stopper::apply()
{
    SLM_TRACE_FUNC();
    ::boost::shared_ptr< Bundle >  bundle = Runtime::getDefault()->findBundle(m_identifier);
    OSLM_FATAL_IF("Unable to stop bundle " << m_identifier << ". Not found.", bundle == 0);
    try
    {
        OSLM_INFO("Stopping bundle : " << m_identifier);
        bundle->stop();
    }
    catch( const std::exception & e )
    {
        OSLM_ERROR("Unable to stop bundle " << m_identifier << ". " << e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace fwRuntime
