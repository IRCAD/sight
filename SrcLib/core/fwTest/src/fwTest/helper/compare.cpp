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

#include "fwTest/helper/compare.hpp"

#include <fwData/Object.hpp>
#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <boost/algorithm/string.hpp>


namespace fwTest
{
namespace helper
{

bool compare(
    ::fwData::Object::sptr objRef,
    ::fwData::Object::sptr objComp,
    ExcludeSetType excludeCompare,
    ExcludeSetType excludeByPrefix)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();

    for(const ExcludeSetType::value_type& key: excludeCompare)
    {
        props->erase(key);
    }

    std::set< ::fwDataCamp::visitor::CompareObjects::PropsMapType::key_type > propsKey;
    for(const ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type &prop: *props)
    {
        bool erased = false;
        for(const ExcludeSetType::value_type& key: excludeByPrefix)
        {
            if(::boost::starts_with(prop.first, key))
            {
                propsKey.insert(prop.first);
                erased = true;
            }
        }

        OSLM_ERROR_IF("new object difference found : " << prop.first << " '" << prop.second << "'", !erased);
    }

    for(const ::fwDataCamp::visitor::CompareObjects::PropsMapType::key_type &key: propsKey)
    {
        props->erase(key);
    }

    return props->empty();
}

} // namespace helper
} // namespace fwTest


