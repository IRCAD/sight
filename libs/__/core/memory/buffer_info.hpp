/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "core/config.hpp"
#include "core/memory/buffer_allocation_policy.hpp"
#include "core/memory/file_format.hpp"
#include "core/memory/file_holder.hpp"
#include "core/memory/stream/in/factory.hpp"

#include <core/logic_stamp.hpp>
#include <core/macros.hpp>

#include <filesystem>

namespace sight::core::memory
{

struct CORE_CLASS_API buffer_info
{
    typedef std::size_t size_t;
    typedef WPTR(void) counter_type;

    CORE_API buffer_info();

    CORE_API void clear();
    //------------------------------------------------------------------------------

    std::int64_t lock_count() const
    {
        return lock_counter.use_count();
    }

    size_t size {0};
    /// true if 'buffer' is loaded
    bool loaded {true};
    bool user_stream_factory {false};

    file_holder fs_file;
    file_format_type file_format {core::memory::OTHER};

    counter_type lock_counter;

    core::logic_stamp last_access;
    core::memory::buffer_allocation_policy::sptr buffer_policy;

    SPTR(core::memory::stream::in::factory) istream_factory;
};

} // namespace sight::core::memory
