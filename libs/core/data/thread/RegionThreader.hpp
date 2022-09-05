/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <thread>
#include <vector>

namespace sight::data::thread
{

class RegionThreader
{
public:

    RegionThreader() :
        m_nbThread((std::thread::hardware_concurrency() > 1) ? std::thread::hardware_concurrency() : 1)
    {
    }

    RegionThreader(std::size_t nbThread, bool capped = true) :
        m_nbThread(std::min(capped ? std::thread::hardware_concurrency() : std::numeric_limits<std::size_t>::max(),
                            (nbThread > 1) ? nbThread : 1))
    {
    }

    //------------------------------------------------------------------------------

    template<typename T>
    void operator()(T func, const std::ptrdiff_t dataSize)
    {
        std::vector<std::thread> threads;

        const std::ptrdiff_t step  = (dataSize / static_cast<std::ptrdiff_t>(m_nbThread)) + 1;
        std::ptrdiff_t regionBegin = 0;
        std::size_t threadId       = 0;

        if(m_nbThread > 1)
        {
            for( ; regionBegin < dataSize ; regionBegin += step, ++threadId)
            {
                threads.push_back(
                    std::thread(
                        func,
                        regionBegin,
                        std::min(dataSize, regionBegin + step),
                        threadId
                    )
                );
            }

            for(auto& thread : threads)
            {
                thread.join();
            }

            threads.clear();
        }
        else
        {
            func(0, dataSize, 0);
        }
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::size_t numberOfThread() const
    {
        return m_nbThread;
    }

protected:

    const std::size_t m_nbThread;
};

} // namespace sight::data::thread
