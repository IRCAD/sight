/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "reset.hpp"

namespace sight::module::data
{

//-----------------------------------------------------------------------------

void reset::configuring(const config_t& _config)
{
    // Check if using batch mode (groups) or singular mode
    const auto inout_node = _config.get_child_optional("inout");
    SIGHT_ASSERT("'inout' tag is required.", inout_node.has_value());

    const auto group_attr    = inout_node->get_optional<std::string>("<xmlattr>.group");
    const bool is_batch_mode = group_attr.has_value();

    if(!is_batch_mode)
    {
        // Singular mode: 'inout' key is required
        SIGHT_ASSERT(
            "'inout' key attribute is required in singular mode.",
            inout_node->get_optional<std::string>("<xmlattr>.key").has_value()
        );
    }
}

//-----------------------------------------------------------------------------

void reset::starting()
{
}

//-----------------------------------------------------------------------------

void reset::updating()
{
    // Determine which mode to use: check if targets group is available
    if(not m_targets.empty())
    {
        this->reset_batch();
        return;
    }

    this->reset_single();
}

//-----------------------------------------------------------------------------

void reset::reset_single()
{
    const auto target = m_target.lock();
    SIGHT_ASSERT("Object empty, can't reset it.", target);

    const auto empty_object = sight::data::factory::make(target->get_classname());
    target->deep_copy(empty_object);
    target->async_emit(sight::data::signals::MODIFIED);
}

//-----------------------------------------------------------------------------

void reset::reset_batch()
{
    const auto num_targets = m_targets.size();

    // Reset each target object
    for(size_t i = 0 ; i < num_targets ; ++i)
    {
        const auto target = m_targets[i].lock();

        if(target)
        {
            const auto empty_object = sight::data::factory::make(target->get_classname());
            target->deep_copy(empty_object);
            target->async_emit(this, sight::data::signals::MODIFIED);
        }
    }
}

//-----------------------------------------------------------------------------

void reset::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
