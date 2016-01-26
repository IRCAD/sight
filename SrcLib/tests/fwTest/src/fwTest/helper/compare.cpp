/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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


