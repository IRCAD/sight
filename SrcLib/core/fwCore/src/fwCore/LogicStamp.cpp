/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwCore/mt/types.hpp"
#include "fwCore/LogicStamp.hpp"

namespace fwCore
{


void LogicStamp::modified()
{
    static LogicStampType globalLogicStamp = 0;
    static ::fwCore::mt::Mutex globalLogicStampMutex;

    ::fwCore::mt::ScopedLock lock(globalLogicStampMutex);

    m_modifiedLogicalTime = ++globalLogicStamp;

    SLM_ASSERT("globalLogicStamp not instanced", globalLogicStamp); // detects overflow
}


} //namespace fwCore
