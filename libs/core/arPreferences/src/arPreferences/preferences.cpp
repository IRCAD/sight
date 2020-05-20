/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "arPreferences/preferences.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/registry/ObjectService.hpp>

namespace arPreferences
{

const std::string s_VIDEO_DIR_PREF = "VIDEO_DIR_PREF";

//-----------------------------------------------------------------------------

std::string getVideoDir()
{
    std::string videoDirectory;

    ::fwData::Composite::sptr prefComposite = ::fwPreferences::getPreferences();

    if(prefComposite)
    {
        ::fwData::Composite::IteratorType iterVideoDir = prefComposite->find( s_VIDEO_DIR_PREF );
        const bool videoFound = (iterVideoDir != prefComposite->end());
        if (videoFound)
        {
            ::fwData::String::sptr videoDir = ::fwData::String::dynamicCast(iterVideoDir->second);
            SLM_ERROR_IF("Wrong type of preference : '" + s_VIDEO_DIR_PREF + "' parameter must be a string",
                         !videoDir);
            videoDirectory = videoDir->value();
        }
    }
    return videoDirectory;
}

//-----------------------------------------------------------------------------

} // namespace arPreferences
