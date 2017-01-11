/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwLog/Log.hpp"

namespace fwLog
{

Log::Log(LevelType level, const std::string& message) : m_level(level), m_message(message)
{
}

//-----------------------------------------------------------------------------

Log::~Log()
{
}

//-----------------------------------------------------------------------------

} //namespace fwLog


