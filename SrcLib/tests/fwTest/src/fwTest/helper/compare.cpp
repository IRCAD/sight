/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTest/helper/compare.hpp"

#include <fwData/Object.hpp>
#include <fwDataCamp/visitor/CompareObjects.hpp>


namespace fwTest
{
namespace helper
{

bool compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp, ExcludeSetType excludeCompare)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for(const ExcludeSetType::value_type& key :  excludeCompare)
    {
        props->erase(key);
    }
    for( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    return props->empty();
}

} // namespace helper
} // namespace fwTest


