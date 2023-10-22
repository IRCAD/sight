/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

//cspell: ignore TABID

#include "activity/message.hpp"

#include <core/tools/uuid.hpp>

#include <data/composite.hpp>
#include <data/string.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <regex>

namespace sight::activity
{

//-----------------------------------------------------------------------------

message::message(
    const data::activity::sptr& _activity,
    const activity::extension::activity_info& _info,
    const parameters_t& _parameters
) :
    m_title(_info.title),
    m_tabID("TABID_" + core::tools::UUID::generate()),
    m_appConfigID(_info.appConfig.id),
    m_tabInfo(_info.tabInfo.empty() ? _info.title : _info.tabInfo),
    m_iconPath(_info.icon),
    m_tooltip(m_tabInfo),
    m_activity(_activity)
{
    SIGHT_ASSERT("Activity instantiation failed", _activity);

    m_replacementMap = sight::activity::extension::activity::getDefault()->getReplacementMap(
        *_activity,
        _info,
        _parameters
    );
}

//-----------------------------------------------------------------------------

} // namespace sight::activity
