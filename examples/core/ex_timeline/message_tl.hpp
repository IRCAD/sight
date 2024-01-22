/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <data/generic_tl.hpp>
#include <data/generic_tl.hxx>
#include <data/timeline/generic_object.hpp>
#include <data/timeline/generic_object.hxx>

#include <array>

namespace ex_timeline
{

struct msg_data
{
    /// Defines the maximum size of a message.
    static const std::size_t MAX_MSG_SIZE = 255;

    /// Defines the identifier of the sender.
    unsigned int uid_sender;

    /// Stores the message as a std::array.
    std::array<char, MAX_MSG_SIZE> sz_msg;
};

/**
 * @brief Defines a timeline that stores string messages.
 */
class message_tl : public sight::data::generic_tl<msg_data>
{
public:

    SIGHT_DECLARE_CLASS(message_tl, sight::data::timeline::base);

    /**
     * @brief Creates the data.
     */
    message_tl() :
        generic_tl<msg_data>()
    {
    }
};

} // namespace ex_timeline
