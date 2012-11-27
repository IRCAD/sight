/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCom/SlotBase.hpp"
#include "fwCom/SlotBase.hxx"

#include "fwCom/SignalBase.hpp"

namespace fwCom
{

#ifdef COM_LOG
size_t SignalBase::s_idCount = 0;
::fwCore::mt::Mutex SignalBase::s_mutexCounter;
#endif

} // namespace fwCom

