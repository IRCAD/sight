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

SIGHT_REGISTER_ACTIVITY_BUILDER(sight::activity::builder::activity, "sight::activity::builder::Activity");

//-----------------------------------------------------------------------------

data::composite::sptr vector_to_composite(
    const data::vector::csptr& _vector,
    const sight::activity::extension::activity_requirement& _req
)
{
    namespace ActReg = sight::activity::extension;
    data::composite::sptr composite = std::make_shared<data::composite>();

    SIGHT_ASSERT("Each possible items in requirement need to have a matching key", _req.keys.size() >= _req.max_occurs);

    auto iter = _req.keys.begin();

    for(const auto& obj : *_vector)
    {
        const ActReg::activity_requirement_key& key_tag = (*iter++);
        (*composite)[key_tag.key] = obj;
    }

    return composite;
}

//-----------------------------------------------------------------------------

data::activity::sptr activity::build_data(
    const sight::activity::extension::activity_info& _activity_info,
    const data::vector::csptr& _current_selection
) const
{
    auto activity = std::make_shared<data::activity>();

    activity->set_activity_config_id(_activity_info.id);
    activity->set_description(_activity_info.description);

    namespace ActReg = sight::activity::extension;

    ActReg::activity_info::requirements_t req_vect = _activity_info.requirements;
    for(const ActReg::activity_requirement& req : req_vect)
    {
        data::vector::sptr vector_type = this->type(_current_selection, req.type);
        // param is optional (minOccurs==0) or required (minOccurs==1), but is single (maxOccurs == 1)
        if(req.max_occurs == 1 && req.min_occurs == 1)
        {
            SIGHT_ASSERT("No param name " << req.name << " with type " << req.type, !vector_type->empty());
            (*activity)[req.name] = (*vector_type)[0];
        }
        else if(req.create || (req.min_occurs == 0 && req.max_occurs == 0))
        {
            (*activity)[req.name] = sight::activity::detail::data::create(req.type, req.object_config);
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
                (*activity)[req.name] = vector_type;
            }
            else if(req.container == "composite" || req.container.empty())
            {
                (*activity)[req.name] = vector_to_composite(vector_type, req);
            }
        }
    }

    return activity;
}

} // namespace sight::activity::builder
