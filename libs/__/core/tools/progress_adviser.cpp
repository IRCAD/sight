/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/tools/progress_adviser.hpp"

namespace sight::core::tools
{

progress_adviser::progress_adviser()
= default;

progress_adviser::~progress_adviser()
= default;

//------------------------------------------------------------------------------

void progress_adviser::add_handler(const progress_handler& _handler)
{
    // append a new ProgressHandler
    m_progress_signal.connect(_handler);
}

//------------------------------------------------------------------------------

void progress_adviser::notify_progress(float _percent, const std::string& _msg) const
{
    m_progress_signal(_percent, _msg);
}

} // namespace sight::core::tools
