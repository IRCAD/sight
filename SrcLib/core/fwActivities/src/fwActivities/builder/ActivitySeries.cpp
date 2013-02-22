/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Vector.hpp>
#include <fwData/Composite.hpp>

#include "fwActivities/builder/registry/macros.hpp"
#include "fwActivities/builder/ActivitySeries.hpp"

namespace fwActivities
{
namespace builder
{

fwActivitiesBuilderRegisterMacro(::fwActivities::builder::ActivitySeries, "::fwActivities::builder::ActivitySeries");

//-----------------------------------------------------------------------------

ActivitySeries::ActivitySeries(::fwActivities::IBuilder::Key key)
{}

//-----------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
{}

//-----------------------------------------------------------------------------

::fwActivities::ActivitySeries::sptr ActivitySeries::buildData(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        ::fwData::Vector::sptr currentSelection ) const
{
    ::fwActivities::ActivitySeries::sptr actSeries = ::fwActivities::ActivitySeries::New();
    actSeries->setActivityConfigId(activityInfo.id);
    ::fwData::Composite::sptr data = actSeries->getData();

    namespace ActReg = ::fwActivities::registry;

    ActReg::ActivityInfo::RequirementsType reqVect = activityInfo.requirements;
    BOOST_FOREACH(const ActReg::ActivityRequirement& req, reqVect)
    {
        ::fwData::Vector::sptr vectorType = this->getType(currentSelection, req.type);
        // param is optional (minOccurs==0) or required (minOccurs==1), but is single (maxOccurs == 1)
        if(req.maxOccurs == 1) //required parameter
        {
            if(req.minOccurs == 1)
            {
                OSLM_ASSERT("No param name "<<req.name<<" with type "<<req.type, !vectorType->empty());
                (*data)[req.name] = (*vectorType)[0];
            }
            else //optional single parameter
            {
                (*data)[req.name] = vectorType;
            }
        }
        // param is a set of data (maxOccurs>1) optional (minOccurs==0) or not (minOccurs==1)
        else if(req.maxOccurs > 1)
        {
            (*data)[req.name] = vectorType;
        }
    }

    return actSeries;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwActivities



