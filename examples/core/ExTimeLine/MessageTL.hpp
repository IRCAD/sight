/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

namespace ExTimeLine
{

struct MsgData
{
    /// Defines the maximum size of a message.
    static const std::size_t MAX_MSG_SIZE = 255;

    /// Defines the identifier of the sender.
    unsigned int uidSender;

    /// Stores the message as a std::array.
    std::array<char, MAX_MSG_SIZE> szMsg;
};

/**
 * @brief Defines a timeline that stores string messages.
 */
class MessageTL : public sight::data::generic_tl<MsgData>
{
public:

    SIGHT_DECLARE_CLASS(MessageTL, sight::data::timeline::base);

    /**
     * @brief Creates the data.
     * @param _key private construction key.
     */
    MessageTL() :
        generic_tl<MsgData>()
    {
    }
};

} // namespace ExTimeLine
