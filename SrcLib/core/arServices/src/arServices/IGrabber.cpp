/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arServices/IGrabber.hpp"

namespace arServices
{

const ::fwCom::Signals::SignalKeyType IGrabber::s_POSITION_MODIFIED_SIG = "positionModified";
const ::fwCom::Signals::SignalKeyType IGrabber::s_DURATION_MODIFIED_SIG = "durationModified";

const ::fwCom::Slots::SlotKeyType IGrabber::s_START_CAMERA_SLOT       = "startCamera";
const ::fwCom::Slots::SlotKeyType IGrabber::s_STOP_CAMERA_SLOT        = "stopCamera";
const ::fwCom::Slots::SlotKeyType IGrabber::s_PAUSE_CAMERA_SLOT       = "pauseCamera";
const ::fwCom::Slots::SlotKeyType IGrabber::s_LOOP_VIDEO_SLOT         = "loopVideo";
const ::fwCom::Slots::SlotKeyType IGrabber::s_SET_POSITION_VIDEO_SLOT = "setPositionVideo";

// ----------------------------------------------------------------------------

IGrabber::IGrabber() throw ()
{

}

// ----------------------------------------------------------------------------

IGrabber::~IGrabber() throw ()
{

}

// ----------------------------------------------------------------------------

}  // namespace arServices
