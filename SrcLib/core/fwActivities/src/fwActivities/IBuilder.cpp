/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Vector.hpp>

#include "fwActivities/IBuilder.hpp"


namespace fwActivities
{

//------------------------------------------------------------------------------

::fwData::Vector::sptr IBuilder::getType( ::fwData::Vector::sptr currentSelection, std::string type ) const
{
    SLM_ASSERT("currentSelection not instanced", currentSelection);

    ::fwData::Vector::sptr subSelection = ::fwData::Vector::New();
    BOOST_FOREACH(::fwData::Vector::value_type obj, *currentSelection)
    {
        OSLM_ASSERT("Object not instanced in selection", obj);
        if(obj->isA(type))
        {
            subSelection->getContainer().push_back(obj);
        }
    }
    return subSelection;
}

//------------------------------------------------------------------------------

} // namespace fwActivities

