/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "activity/builder/activity.hpp"

#include "activity/builder/data.hpp"
#include "activity/builder/registry/macros.hpp"

#include <data/composite.hpp>
#include <data/vector.hpp>

namespace sight::activity::builder
{

SIGHT_REGISTER_ACTIVITY_BUILDER(sight::activity::builder::Activity, "sight::activity::builder::Activity");

//-----------------------------------------------------------------------------

data::composite::sptr vectorToComposite(
    const data::vector::csptr& vector,
    const activity::extension::activity_requirement& req
)
{
    namespace ActReg = activity::extension;
    data::composite::sptr composite = std::make_shared<data::composite>();

    SIGHT_ASSERT("Each possible items in requirement need to have a matching key", req.keys.size() >= req.maxOccurs);

    auto iter = req.keys.begin();

    for(const auto& obj : *vector)
    {
        const ActReg::activity_requirement_key& keyTag = (*iter++);
        (*composite)[keyTag.key] = obj;
    }

    return composite;
}

//-----------------------------------------------------------------------------

data::activity::sptr Activity::buildData(
    const activity::extension::activity_info& activity_info,
    const data::vector::csptr& currentSelection
) const
{
    auto activity = std::make_shared<data::activity>();

    activity->setActivityConfigId(activity_info.id);
    activity->setDescription(activity_info.description);

    namespace ActReg = activity::extension;

    ActReg::activity_info::RequirementsType reqVect = activity_info.requirements;
    for(const ActReg::activity_requirement& req : reqVect)
    {
        data::vector::sptr vectorType = this->getType(currentSelection, req.type);
        // param is optional (minOccurs==0) or required (minOccurs==1), but is single (maxOccurs == 1)
        if(req.maxOccurs == 1 && req.minOccurs == 1)
        {
            SIGHT_ASSERT("No param name " << req.name << " with type " << req.type, !vectorType->empty());
            (*activity)[req.name] = (*vectorType)[0];
        }
        else if(req.create || (req.minOccurs == 0 && req.maxOccurs == 0))
        {
            (*activity)[req.name] = sight::activity::detail::data::create(req.type, req.objectConfig);
        }
        else
        {
            SIGHT_ASSERT(
                "Unknown specified container: '" + req.container + "'.",
                req.container.empty()
                || req.container == "vector"
                || req.container == "composite"
            );
            if(req.container == "vector")
            {
                (*activity)[req.name] = vectorType;
            }
            else if(req.container == "composite" || req.container.empty())
            {
                (*activity)[req.name] = vectorToComposite(vectorType, req);
            }
        }
    }

    return activity;
}

} // namespace sight::activity::builder
