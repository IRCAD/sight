/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "core/memory/buffer_info.hpp"

namespace sight::core::memory
{

buffer_info::buffer_info()
{
    this->last_access.modified();
}

//-----------------------------------------------------------------------------

void buffer_info::clear()
{
    size                = 0;
    loaded              = true;
    user_stream_factory = false;
    fs_file.clear();
    file_format = core::memory::OTHER;
    lock_counter.reset();
    last_access = core::logic_stamp();
    buffer_policy.reset();
    istream_factory.reset();
}

} // namespace sight::core::memory
