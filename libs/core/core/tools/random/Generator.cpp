/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "Generator.hpp"

namespace sight::core::tools
{

namespace random
{

static std::mutex s_mutex;

/// This is a thread safe implementation
int safeRand(const bool reset)
{
    std::lock_guard guard(s_mutex);

    static std::mt19937 s_generator(1);
    static std::uniform_int_distribution<int> s_distributor(0, RAND_MAX);

    if(reset)
    {
        s_generator = std::mt19937(1);
    }

    return s_distributor(s_generator);
}

}

}
