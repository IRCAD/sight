/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */


#include "fwCore/TimeStamp.hpp"

namespace fwCore
{


void TimeStamp::modified()
{
    m_modifiedTime = ::fwCore::HiResClock::getTimeInMilliSec();
}


} //namespace fwCore

