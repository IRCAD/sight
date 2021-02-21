/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "io/atoms/filter/functions.hpp"

#include <activity/extension/Activity.hpp>

#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <data/registry/detail.hpp>

#include <io/atoms/patch/helper/functions.hpp>

#include <algorithm>
#include <string>

namespace sight::io::atoms::filter
{

//------------------------------------------------------------------------------

bool isSeriesKnown(const SPTR(sight::atoms::Object)& series)
{
    bool isKnown = false;

    namespace reg = activity::extension;
    reg::Activity::sptr registry                  = reg::Activity::getDefault();
    const std::vector< reg::ActivityInfo >& infos = registry->getInfos();

    const std::string& classname = io::atoms::patch::helper::getClassname(series);

    SPTR(data::registry::Type) objectRegistry = data::registry::get();
    data::registry::Type::KeyVectorType keys            = objectRegistry->getFactoryKeys();
    data::registry::Type::KeyVectorType::iterator itKey = std::find(keys.begin(), keys.end(), classname);

    if(itKey != keys.end())
    {
        if(classname == "::sight::data::ActivitySeries")
        {
            // Check if activity config id is known
            sight::atoms::String::sptr asId = series->getAttribute< sight::atoms::String >("activity_config_id");

            for(reg::ActivityInfo info :  infos)
            {
                if(info.id == asId->getString())
                {
                    isKnown = true;
                    break;
                }
            }
        }
        else
        {
            isKnown = true;
        }
    }

    return isKnown;
}

} // namespace sight::io::atoms::filter
