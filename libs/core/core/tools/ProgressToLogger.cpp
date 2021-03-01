/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "core/tools/ProgressToLogger.hpp"

#include <core/base.hpp>

namespace sight::core::tools
{

ProgressToLogger::ProgressToLogger(const std::string& prefix, const std::string& postfix) :
    m_prefix(prefix),
    m_postfix(postfix)
{
}

ProgressToLogger::~ProgressToLogger()
{
    // TODO Auto-generated destructor stub
}

//------------------------------------------------------------------------------

void ProgressToLogger::operator()(float percent, const std::string& msg)
{
#if !SIGHT_INFO_ENABLED
    SIGHT_NOT_USED(percent);
    SIGHT_NOT_USED(msg);
#endif
    // fixme io precision
    SIGHT_INFO( m_prefix << msg << " " << (int) (percent*100) << m_postfix);
}

}
