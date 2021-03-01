/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "ui/base/preferences/preferences.hpp"

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <service/registry/ObjectService.hpp>

#include <ui/base/preferences/helper.hpp>

namespace sight::ui::base::preferences
{

const std::string s_VIDEO_DIR_PREF = "VIDEO_DIR_PREF";

//-----------------------------------------------------------------------------

std::string getVideoDir()
{
    std::string videoDirectory;

    data::Composite::sptr prefComposite = ui::base::preferences::getPreferences();

    if(prefComposite)
    {
        data::Composite::IteratorType iterVideoDir = prefComposite->find( s_VIDEO_DIR_PREF );
        const bool videoFound                      = (iterVideoDir != prefComposite->end());
        if (videoFound)
        {
            data::String::sptr videoDir = data::String::dynamicCast(iterVideoDir->second);
            SIGHT_ERROR_IF("Wrong type of preference : '" + s_VIDEO_DIR_PREF + "' parameter must be a string",
                           !videoDir);
            videoDirectory = videoDir->value();
        }
    }
    return videoDirectory;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base::preferences
