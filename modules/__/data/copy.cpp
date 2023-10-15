/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
    SIGHT_ASSERT("One 'in' tag is required.", config.get_optional<std::string>("in").has_value());

    [[maybe_unused]] const auto inoutCfg = config.get_optional<std::string>("inout");
    [[maybe_unused]] const auto outCfg   = config.get_optional<std::string>("out");
    SIGHT_ASSERT("One 'inout' or one 'out' tag is required.", inoutCfg.has_value() + outCfg.has_value());

    if(const auto modeConfig = config.get_optional<std::string>("mode"); modeConfig.has_value())
    {
        const auto& mode = modeConfig.value();
        if(mode == "copyOnStart")
        {
            m_mode = ModeType::START;
        }
        else if(mode == "copyOnUpdate")
        {
            m_mode = ModeType::UPDATE;
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
    if(m_mode == ModeType::START)
    {
        this->make_copy();
    }
}

//-----------------------------------------------------------------------------

void copy::updating()
{
    if(m_mode == ModeType::UPDATE)
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
    m_outTarget = nullptr;
}

//-----------------------------------------------------------------------------

void copy::make_copy()
{
    // Check if we use inout or output.
    bool create = false;
    {
        const auto targetLock = m_target.lock();
        if(!targetLock)
        {
            create = true;
        }
    }

    // Extract the object.
    const auto sourceObject = m_source.lock();

    sight::data::object::csptr source = sourceObject.get_shared();

    if(source)
    {
        const auto setOutputData =
            [&]()
            {
                if(create)
                {
                    // Set the data as output.
                    sight::data::object::sptr target = sight::data::object::copy(source);
                    m_outTarget = target;
                }
                else
                {
                    // Copy the object to the inout.
                    const auto target = m_target.lock();
                    target->deep_copy(source);

                    auto sig = target->signal<sight::data::object::ModifiedSignalType>(
                        sight::data::object::MODIFIED_SIG
                    );
                    {
                        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                        sig->async_emit();
                    }
                }
            };

        setOutputData();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
