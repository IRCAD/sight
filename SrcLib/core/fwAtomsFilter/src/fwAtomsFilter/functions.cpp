/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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


#include "fwAtomsFilter/functions.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

#include <fwData/registry/detail.hpp>

#include <algorithm>
#include <string>

namespace fwAtomsFilter
{

bool isSeriesKnown(const SPTR(::fwAtoms::Object)& series)
{
    bool isKnown = false;

    namespace reg = ::fwActivities::registry;
    reg::Activities::sptr registry                = reg::Activities::getDefault();
    const std::vector< reg::ActivityInfo >& infos = registry->getInfos();

    const std::string& classname = ::fwAtomsPatch::helper::getClassname(series);

    SPTR(::fwData::registry::Type) objectRegistry           = ::fwData::registry::get();
    ::fwData::registry::Type::KeyVectorType keys            = objectRegistry->getFactoryKeys();
    ::fwData::registry::Type::KeyVectorType::iterator itKey = std::find(keys.begin(), keys.end(), classname);

    if(itKey != keys.end())
    {
        if(classname == "::fwMedData::ActivitySeries")
        {
            // Check if activity config id is known
            ::fwAtoms::String::sptr asId = series->getAttribute< ::fwAtoms::String >("activity_config_id");

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

} // namespace fwAtomsFilter

