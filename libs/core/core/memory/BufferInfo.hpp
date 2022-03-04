/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "core/memory/BufferAllocationPolicy.hpp"
#include "core/memory/FileFormat.hpp"
#include "core/memory/FileHolder.hpp"
#include "core/memory/stream/in/IFactory.hpp"

#include <core/LogicStamp.hpp>
#include <core/macros.hpp>

#include <filesystem>

namespace sight::core::memory
{

struct CORE_CLASS_API BufferInfo
{
    typedef std::size_t SizeType;
    typedef WPTR(void) CounterType;

    CORE_API BufferInfo();

    CORE_API void clear();
    //------------------------------------------------------------------------------

    long lockCount() const
    {
        return lockCounter.use_count();
    }

    SizeType size;
    /// true if 'buffer' is loaded
    bool loaded;
    bool userStreamFactory;

    FileHolder fsFile;
    FileFormatType fileFormat;

    CounterType lockCounter;

    core::LogicStamp lastAccess;
    core::memory::BufferAllocationPolicy::sptr bufferPolicy;

    SPTR(core::memory::stream::in::IFactory) istreamFactory;
};

} // namespace sight::core::memory
