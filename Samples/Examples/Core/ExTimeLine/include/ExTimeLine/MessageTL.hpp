/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "ExTimeLine/config.hpp"

#include <arData/GenericTL.hpp>
#include <arData/GenericTL.hxx>
#include <arData/timeline/GenericObject.hpp>
#include <arData/timeline/GenericObject.hxx>

namespace ExTimeLine
{

struct MsgData
{
    /// Maximum size of a message
    static const size_t MAX_MSG_SIZE = 255;

    /// Identifier of the sender
    unsigned int uidSender;
    /// Message as a C string
    char szMsg[MAX_MSG_SIZE];
};

/**
 * @brief   This class defines a timeline that stores string messages.
 */
class EXTIMELINE_CLASS_API MessageTL : public ::arData::GenericTL< MsgData >
{
public:
    fwCoreClassMacro(MessageTL, ::arData::TimeLine, ::fwData::factory::New< MessageTL >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    MessageTL( ::fwData::Object::Key key ) :
        GenericTL< MsgData >(key)
    {
    }
};

} // namespace ExTimeLine
