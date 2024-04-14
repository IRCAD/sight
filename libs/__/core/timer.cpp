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

#include "core/timer.hpp"

#include <cstdlib>

namespace sight::core
{

timer::timer()
{
    this->reset();
}

timer::~timer()
= default;

//------------------------------------------------------------------------------

void timer::start()
{
    m_stopped = false;

    m_cumul_time_in_micro_sec = m_end_time_in_micro_sec - m_start_time_in_micro_sec;
    m_start_time_in_micro_sec = core::clock::get_time_in_micro_sec();
}

//------------------------------------------------------------------------------

void timer::stop()
{
    m_stopped = true;

    m_end_time_in_micro_sec = core::clock::get_time_in_micro_sec();
}

//------------------------------------------------------------------------------

void timer::reset(core::clock::type _initial_value)
{
    bool was_stopped = m_stopped;
    if(!was_stopped)
    {
        this->stop();
    }

    m_start_time_in_micro_sec = 0.;
    m_end_time_in_micro_sec   = 0.;
    m_cumul_time_in_micro_sec = _initial_value;
    if(!was_stopped)
    {
        this->start();
    }
}

//------------------------------------------------------------------------------

core::clock::type timer::get_elapsed_time_in_micro_sec()
{
    if(!m_stopped)
    {
        m_end_time_in_micro_sec = core::clock::get_time_in_micro_sec();
    }

    return m_cumul_time_in_micro_sec + (m_end_time_in_micro_sec - m_start_time_in_micro_sec);
}

//------------------------------------------------------------------------------

core::clock::type timer::get_elapsed_time_in_milli_sec()
{
    return this->get_elapsed_time_in_micro_sec() * 0.001;
}

//------------------------------------------------------------------------------

core::clock::type timer::get_elapsed_time_in_sec()
{
    return this->get_elapsed_time_in_micro_sec() * 0.000001;
}

} //namespace sight::core
