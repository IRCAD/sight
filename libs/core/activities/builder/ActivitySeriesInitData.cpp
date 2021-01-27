/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "activities/builder/ActivitySeriesInitData.hpp"

#include "activities/builder/registry/macros.hpp"

#include <data/Composite.hpp>
#include <data/Vector.hpp>

namespace sight::activities
{
namespace builder
{

fwActivitiesBuilderRegisterMacro(
    activities::builder::ActivitySeriesInitData, "::activities::builder::ActivitySeriesInitData");

//-----------------------------------------------------------------------------

ActivitySeriesInitData::ActivitySeriesInitData(activities::IBuilder::Key key) :
    activities::builder::ActivitySeries(key)
{
}

//-----------------------------------------------------------------------------

ActivitySeriesInitData::~ActivitySeriesInitData()
{
}

//-----------------------------------------------------------------------------

data::ActivitySeries::sptr ActivitySeriesInitData::buildData(
    const activities::registry::ActivityInfo& activityInfo,
    const data::Vector::csptr& currentSelection ) const
{
    data::ActivitySeries::sptr actSeries = this->ActivitySeries::buildData(activityInfo, currentSelection);
    data::Composite::sptr data           = actSeries->getData();

    namespace ActReg = activities::registry;

    ActReg::ActivityInfo::RequirementsType reqVect = activityInfo.requirements;
    for(const ActReg::ActivityRequirement& req :  reqVect)
    {
        if(req.maxOccurs == 0 && req.minOccurs == 0)
        {
            (*data)[req.name] = data::factory::New(req.type);
        }
    }

    return actSeries;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace sight::activities
