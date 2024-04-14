/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/tools/progress_to_logger.hpp"

#include <core/base.hpp>

#include <utility>

namespace sight::core::tools
{

progress_to_logger::progress_to_logger(std::string _prefix, std::string _postfix) :
    m_prefix(std::move(_prefix)),
    m_postfix(std::move(_postfix))
{
}

progress_to_logger::~progress_to_logger()
= default;

//------------------------------------------------------------------------------

void progress_to_logger::operator()(float _percent, const std::string& _msg)
{
#if !SIGHT_INFO_ENABLED
    SIGHT_NOT_USED(_percent);
    SIGHT_NOT_USED(_msg);
#endif
    // fixme io precision
    SIGHT_INFO(m_prefix << _msg << " " << (int) (_percent * 100) << m_postfix);
}

} // namespace sight::core::tools
