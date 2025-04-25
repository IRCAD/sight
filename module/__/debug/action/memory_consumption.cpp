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

#include "memory_consumption.hpp"

#include <data/array.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <boost/lexical_cast.hpp>

#include <sstream>

namespace sight::module::debug::action
{

/// Static variable shared by both actions
static std::vector<data::array::sptr> memory_consumer;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void memory_consumption::push_new_array(std::size_t _memory_size_in_bytes)
{
    try
    {
        data::array::sptr buffer = std::make_shared<data::array>();
        data::array::size_t size(1, _memory_size_in_bytes);
        buffer->resize(size, core::type::UINT8, true);

        SIGHT_INFO("Creating a data::array consuming " << _memory_size_in_bytes / (1024LL * 1024) << " Mo ");

        memory_consumer.push_back(buffer);
    }
    catch(std::exception& e)
    {
        std::stringstream msg;
        msg << "Cannot allocate buffer (" << _memory_size_in_bytes / (1024LL * 1024) << " Mo) :\n" << e.what()
        << std::endl;
        sight::ui::dialog::message::show(
            "Action increase memory",
            msg.str(),
            sight::ui::dialog::message::critical
        );
    }
}

//------------------------------------------------------------------------------

memory_consumption::memory_consumption() noexcept :

    m_memory_size_in_bytes(1024LL * 1024 * 256) // 256 Mo
{
}

//------------------------------------------------------------------------------

memory_consumption::~memory_consumption() noexcept =
    default;

//------------------------------------------------------------------------------

void memory_consumption::updating()
{
    if(m_is_increase_mode)
    {
        this->push_new_array(m_memory_size_in_bytes);
    }
    else
    {
        if(!memory_consumer.empty())
        {
            SIGHT_INFO("Removing one data::array");
            memory_consumer.pop_back();
        }
    }
}

//------------------------------------------------------------------------------

void memory_consumption::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->get_config();

    const auto mode = config.get<std::string>("config.<xmlattr>.mode");
    SIGHT_ASSERT("Wrong value (" << mode << ") for mode tag", mode == "increase" || mode == "decrease");
    m_is_increase_mode = (mode == "increase");

    if(const auto value = config.get_optional<std::string>("config.<xmlattr>.value");
       m_is_increase_mode&& value.has_value())
    {
        auto size_in_mo = boost::lexical_cast<std::size_t>(value.value());
        m_memory_size_in_bytes = size_in_mo * 1024 * 1024;
    }
}

//------------------------------------------------------------------------------

void memory_consumption::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------
void memory_consumption::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
