/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Object.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include "fwTest/helper/compare.hpp"

namespace fwTest
{
namespace helper
{

bool compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    BOOST_FOREACH( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    return props->empty();
}

} // namespace helper
} // namespace fwTest


