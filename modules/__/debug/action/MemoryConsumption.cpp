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

#include "MemoryConsumption.hpp"

#include <data/Array.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <boost/lexical_cast.hpp>

#include <sstream>

namespace sight::module::debug::action
{

/// Static variable shared by both actions
static std::vector<data::Array::sptr> memoryConsumer;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void MemoryConsumption::pushNewArray(std::size_t memorySizeInBytes)
{
    try
    {
        data::Array::sptr buffer = std::make_shared<data::Array>();
        data::Array::SizeType size(1, memorySizeInBytes);
        buffer->resize(size, core::type::UINT8, true);

        SIGHT_INFO("Creating a data::array consuming " << memorySizeInBytes / (1024LL * 1024) << " Mo ");

        memoryConsumer.push_back(buffer);
    }
    catch(std::exception& e)
    {
        std::stringstream msg;
        msg << "Cannot allocate buffer (" << memorySizeInBytes / (1024LL * 1024) << " Mo) :\n" << e.what() << std::endl;
        sight::ui::dialog::message::show(
            "Action increase memory",
            msg.str(),
            sight::ui::dialog::message::CRITICAL
        );
    }
}

//------------------------------------------------------------------------------

MemoryConsumption::MemoryConsumption() noexcept :

    m_memorySizeInBytes(1024LL * 1024 * 256) // 256 Mo
{
}

//------------------------------------------------------------------------------

MemoryConsumption::~MemoryConsumption() noexcept =
    default;

//------------------------------------------------------------------------------

void MemoryConsumption::updating()
{
    if(m_isIncreaseMode)
    {
        this->pushNewArray(m_memorySizeInBytes);
    }
    else
    {
        if(!memoryConsumer.empty())
        {
            SIGHT_INFO("Removing one data::Array");
            memoryConsumer.pop_back();
        }
    }
}

//------------------------------------------------------------------------------

void MemoryConsumption::configuring()
{
    this->sight::ui::action::initialize();

    const auto& config = this->getConfiguration();

    const auto mode = config.get<std::string>("config.<xmlattr>.mode");
    SIGHT_ASSERT("Wrong value (" << mode << ") for mode tag", mode == "increase" || mode == "decrease");
    m_isIncreaseMode = (mode == "increase");

    if(const auto value = config.get_optional<std::string>("config.<xmlattr>.value");
       m_isIncreaseMode&& value.has_value())
    {
        auto sizeInMo = boost::lexical_cast<std::size_t>(value.value());
        m_memorySizeInBytes = sizeInMo * 1024 * 1024;
    }
}

//------------------------------------------------------------------------------

void MemoryConsumption::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------
void MemoryConsumption::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
