/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

::fwActivities::ActivitySeries::sptr ActivitySeries::buildData( ::fwData::Composite::sptr currentSelection,
                                                                const ConfigIdType& activityConfigId ) const
{
    ::fwActivities::ActivitySeries::sptr actSeries = ::fwActivities::ActivitySeries::New();
    actSeries->setActivityConfigId(activityConfigId);
    return actSeries;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwActivities



