/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arData/TimeLine.hpp"

#include <fwCom/Signal.hxx>

namespace arData
{

const ::fwCom::Signals::SignalKeyType TimeLine::s_OBJECT_PUSHED_SIG  = "objectPushed";
const ::fwCom::Signals::SignalKeyType TimeLine::s_OBJECT_REMOVED_SIG = "objectRemoved";
const ::fwCom::Signals::SignalKeyType TimeLine::s_CLEARED_SIG        = "objectCleared";

//------------------------------------------------------------------------------

TimeLine::TimeLine ( ::fwData::Object::Key )
{
    newSignal<ObjectPushedSignalType>(s_OBJECT_PUSHED_SIG);
    newSignal<ObjectRemovedSignalType>(s_OBJECT_REMOVED_SIG);
    newSignal<ObjectClearedSignalType>(s_CLEARED_SIG);
}

//------------------------------------------------------------------------------

TimeLine::~TimeLine ()
{
}

//------------------------------------------------------------------------------

} // namespace arData

