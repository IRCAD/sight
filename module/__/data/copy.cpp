/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "copy.hpp"

namespace sight::module::data
{

//-----------------------------------------------------------------------------

void copy::configuring()
{
    const auto& config = this->get_config();

    if(const auto mode_config = config.get_optional<std::string>("mode"); mode_config.has_value())
    {
        const auto& mode = mode_config.value();
        if(mode == "copyOnStart")
        {
            m_mode = mode_t::start;
        }
        else if(mode == "copyOnUpdate")
        {
            m_mode = mode_t::update;
        }
        else
        {
            SIGHT_ERROR("Mode " + mode + " unknown. It should be either 'copyOnStart' or 'copyOnUpdate'");
        }
    }
}

//-----------------------------------------------------------------------------

void copy::starting()
{
    if(m_mode == mode_t::start)
    {
        this->make_copy();
    }
}

//-----------------------------------------------------------------------------

void copy::updating()
{
    if(m_mode == mode_t::update)
    {
        this->make_copy();
    }
    else
    {
        SIGHT_WARN("Object copy was request but the mode is to 'copyOnStart'");
    }
}

//-----------------------------------------------------------------------------

void copy::stopping()
{
}

//-----------------------------------------------------------------------------

void copy::make_copy()
{
    const auto num_sources                  = m_sources.size();
    [[maybe_unused]] const auto num_targets = m_targets.size();

    SIGHT_ASSERT("Source and target groups must have the same number of objects.", num_sources == num_targets);

    // Copy each source to the corresponding target by index
    for(std::size_t i = 0 ; i < num_sources ; ++i)
    {
        const auto source = m_sources[i].lock();
        const auto target = m_targets[i].lock();

        if(source && target)
        {
            target->deep_copy(source.get_shared());
            target->async_emit(this, sight::data::signals::MODIFIED);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
