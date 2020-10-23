/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "exTimeLineData/config.hpp"

#include <arData/GenericTL.hpp>
#include <arData/GenericTL.hxx>
#include <arData/timeline/GenericObject.hpp>
#include <arData/timeline/GenericObject.hxx>

namespace exTimeLineData
{

struct MsgData
{

    /// Defines the maximum size of a message.
    static const size_t MAX_MSG_SIZE = 255;

    /// Defines the identifier of the sender.
    unsigned int uidSender;

    /// Stores the message as a C string.
    char szMsg[MAX_MSG_SIZE];

};

/**
 * @brief Defines a timeline that stores string messages.
 */
class EXTIMELINEDATA_CLASS_API MessageTL : public ::arData::GenericTL< MsgData >
{

public:

    fwCoreClassMacro(MessageTL, ::arData::TimeLine, ::fwData::factory::New< MessageTL >)

    /**
     * @brief Creates the data.
     * @param _key private construction key.
     */
    EXTIMELINEDATA_API MessageTL(::fwData::Object::Key _key) :
        GenericTL< MsgData >(_key)
    {
    }

};

} // namespace exTimeLineData.
