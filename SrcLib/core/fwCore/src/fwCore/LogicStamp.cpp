/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "fwCore/LogicStamp.hpp"

namespace fwCore
{


typedef ::boost::interprocess::interprocess_mutex    mutex;
typedef ::boost::interprocess::scoped_lock< mutex >  scopedLock;

void LogicStamp::modified()
{
    static LogicStampType globalLogicStamp = 0;
    static mutex globalLogicStampMutex;

    scopedLock lock(globalLogicStampMutex);

    m_modifiedLogicalTime = ++globalLogicStamp;

    assert( globalLogicStamp ); // detects overflow
}


} //namespace fwCore
