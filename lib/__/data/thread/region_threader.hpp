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

#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <thread>
#include <vector>

namespace sight::data::thread
{

class region_threader
{
public:

    region_threader() :
        m_nb_thread((std::thread::hardware_concurrency() > 1) ? std::thread::hardware_concurrency() : 1)
    {
    }

    region_threader(std::size_t _nb_thread, bool _capped = true) :
        m_nb_thread(std::min(_capped ? std::thread::hardware_concurrency() : std::numeric_limits<std::size_t>::max(),
                             (_nb_thread > 1) ? _nb_thread : 1))
    {
    }

    //------------------------------------------------------------------------------

    template<typename T>
    void operator()(T _func, const std::ptrdiff_t _data_size)
    {
        std::vector<std::thread> threads;

        const std::ptrdiff_t step = (_data_size / static_cast<std::ptrdiff_t>(m_nb_thread)) + 1;

        if(m_nb_thread > 1)
        {
            std::ptrdiff_t region_begin = 0;
            std::size_t thread_id       = 0;
            for( ; region_begin < _data_size ; region_begin += step, ++thread_id)
            {
                threads.push_back(
                    std::thread(
                        _func,
                        region_begin,
                        std::min(_data_size, region_begin + step),
                        thread_id
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
            _func(0, _data_size, 0);
        }
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::size_t number_of_thread() const
    {
        return m_nb_thread;
    }

protected:

    const std::size_t m_nb_thread;
};

} // namespace sight::data::thread
