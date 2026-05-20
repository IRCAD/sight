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

#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

void copy::configuring()
{
    const auto& config = this->get_config();

    // Check if using batch mode (groups) or singular mode
    const auto in_node = config.get_child_optional("in");
    SIGHT_ASSERT("'in' tag is required.", in_node.has_value());

    const auto group_attr    = in_node->get_optional<std::string>("<xmlattr>.group");
    const bool is_batch_mode = group_attr.has_value();

    if(is_batch_mode)
    {
        // Batch mode: check for inout group
        const auto inout_node = config.get_child_optional("inout");
        SIGHT_ASSERT("'inout' is required in batch mode.", inout_node.has_value());

        const auto inout_group = inout_node->get_optional<std::string>("<xmlattr>.group");
        SIGHT_ASSERT("'inout' with 'group' attribute is required in batch mode.", inout_group.has_value());
    }
    else
    {
        // Singular mode: check for either inout or out
        [[maybe_unused]] const auto inout_cfg = config.get_optional<std::string>("inout");
        [[maybe_unused]] const auto out_cfg   = config.get_optional<std::string>("out");
        SIGHT_ASSERT(
            "One 'inout' or one 'out' tag is required in singular mode.",
            inout_cfg.has_value() + out_cfg.has_value() == 1
        );
    }

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
    // Unregister output
    m_out_target = nullptr;
}

//-----------------------------------------------------------------------------

void copy::make_copy()
{
    // Determine which mode to use: check if sources are available
    if(not m_sources.empty())
    {
        this->make_copy_batch();
        return;
    }

    // Check if we use inout or output.
    bool create = false;
    {
        const auto target = m_target.lock();
        if(!target)
        {
            create = true;
        }
    }

    // Extract the object.
    const auto source_object = m_source.lock();

    sight::data::object::csptr source = source_object.get_shared();

    if(source)
    {
        const auto set_output_data =
            [&]()
            {
                if(create)
                {
                    // Set the data as output.
                    sight::data::object::sptr target = sight::data::object::copy(source);
                    m_out_target = target;
                }
                else
                {
                    // Copy the object to the inout.
                    const auto target = m_target.lock();
                    target->deep_copy(source);
                    target->async_emit(this, sight::data::signals::MODIFIED);
                }
            };

        set_output_data();
    }
}

//-----------------------------------------------------------------------------

void copy::make_copy_batch()
{
    const auto num_sources                  = m_sources.size();
    [[maybe_unused]] const auto num_targets = m_targets.size();

    SIGHT_ASSERT("Source and target groups must have the same number of objects.", num_sources == num_targets);

    // Copy each source to the corresponding target by index
    for(size_t i = 0 ; i < num_sources ; ++i)
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
