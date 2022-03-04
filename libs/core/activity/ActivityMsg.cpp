/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
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

#include "activity/ActivityMsg.hpp"

#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <regex>

namespace sight::activity
{

//-----------------------------------------------------------------------------

ActivityMsg::ActivityMsg(
    const data::ActivitySeries::sptr& series,
    const activity::extension::ActivityInfo& info,
    const ParametersType& parameters
)
{
    SIGHT_ASSERT("ActivitySeries instantiation failed", series);

    m_replacementMap = sight::activity::extension::Activity::getDefault()->getReplacementMap(
        *series,
        info,
        parameters
    );

    //cspell: ignore TABID
    m_title       = info.title;
    m_appConfigID = info.appConfig.id;
    m_closable    = true;
    m_tabID       = "TABID_" + core::tools::UUID::generateUUID();
    m_tabInfo     = info.tabInfo.empty() ? info.title : info.tabInfo;
    m_iconPath    = info.icon;
    m_tooltip     = m_tabInfo;
    m_series      = series;
}

//-----------------------------------------------------------------------------

} // namespace sight::activity
