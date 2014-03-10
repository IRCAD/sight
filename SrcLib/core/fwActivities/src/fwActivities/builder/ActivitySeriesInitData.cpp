/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Vector.hpp>
#include <fwData/Composite.hpp>

#include "fwActivities/builder/registry/macros.hpp"
#include "fwActivities/builder/ActivitySeriesInitData.hpp"

namespace fwActivities
{
namespace builder
{

fwActivitiesBuilderRegisterMacro(
        ::fwActivities::builder::ActivitySeriesInitData, "::fwActivities::builder::ActivitySeriesInitData");

//-----------------------------------------------------------------------------

ActivitySeriesInitData::ActivitySeriesInitData(::fwActivities::IBuilder::Key key)
    : fwActivities::builder::ActivitySeries(key)
{}

//-----------------------------------------------------------------------------

ActivitySeriesInitData::~ActivitySeriesInitData()
{}

//-----------------------------------------------------------------------------

::fwMedData::ActivitySeries::sptr ActivitySeriesInitData::buildData(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        ::fwData::Vector::sptr currentSelection ) const
{
    ::fwMedData::ActivitySeries::sptr actSeries = this->ActivitySeries::buildData(activityInfo, currentSelection);
    ::fwData::Composite::sptr data = actSeries->getData();

    namespace ActReg = ::fwActivities::registry;

    ActReg::ActivityInfo::RequirementsType reqVect = activityInfo.requirements;
    BOOST_FOREACH(const ActReg::ActivityRequirement& req, reqVect)
    {
        if(req.maxOccurs == 0 && req.minOccurs == 0)
        {
            (*data)[req.name] = ::fwData::factory::New(req.type);
        }
    }

    return actSeries;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwActivities

