/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <string>

#include <boost/foreach.hpp>

#include <fwActivities/registry/Activities.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

#include <fwData/registry/detail.hpp>

#include "fwAtomsFilter/functions.hpp"


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

            BOOST_FOREACH(reg::ActivityInfo info, infos)
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

