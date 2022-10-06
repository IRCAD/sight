/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "activity/builder/ActivityInitData.hpp"

#include "activity/builder/registry/macros.hpp"

#include <data/Composite.hpp>
#include <data/Vector.hpp>

namespace sight::activity::builder
{

SIGHT_REGISTER_ACTIVITY_BUILDER(
    activity::builder::ActivityInitData,
    "sight::activity::builder::ActivityInitData"
);

//-----------------------------------------------------------------------------

ActivityInitData::ActivityInitData(activity::IBuilder::Key key) :
    activity::builder::Activity(key)
{
}

//-----------------------------------------------------------------------------

data::Activity::sptr ActivityInitData::buildData(
    const activity::extension::ActivityInfo& activityInfo,
    const data::Vector::csptr& currentSelection
) const
{
    auto activity = this->Activity::buildData(activityInfo, currentSelection);
    auto data     = activity->getData();

    namespace ActReg = activity::extension;

    ActReg::ActivityInfo::RequirementsType reqVect = activityInfo.requirements;
    for(const ActReg::ActivityRequirement& req : reqVect)
    {
        if(req.maxOccurs == 0 && req.minOccurs == 0)
        {
            (*data)[req.name] = data::factory::New(req.type);
        }
    }

    return activity;
}

} // namespace sight::activity::builder
