/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include "arData/TimeLine.hpp"

namespace arData
{

const ::fwCom::Signals::SignalKeyType TimeLine::s_OBJECT_PUSHED_SIG  = "objectPushed";
const ::fwCom::Signals::SignalKeyType TimeLine::s_OBJECT_REMOVED_SIG = "objectRemoved";

//------------------------------------------------------------------------------

TimeLine::TimeLine ( ::fwData::Object::Key key )
{
    m_sigObjectPushed = ObjectPushedSignalType::New();
    m_signals( s_OBJECT_PUSHED_SIG,  m_sigObjectPushed);

    m_sigObjectRemoved = ObjectRemovedSignalType::New();
    m_signals( s_OBJECT_REMOVED_SIG,  m_sigObjectRemoved);
}

//------------------------------------------------------------------------------

TimeLine::~TimeLine ()
{
}

//------------------------------------------------------------------------------

} // namespace arData


