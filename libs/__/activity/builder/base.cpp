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

#include "activity/builder/base.hpp"

#include <data/vector.hpp>

namespace sight::activity::builder
{

//------------------------------------------------------------------------------

data::vector::sptr base::type(const data::vector::csptr& _current_selection, const std::string& _type) const
{
    SIGHT_ASSERT("currentSelection not instanced", _current_selection);

    data::vector::sptr sub_selection = std::make_shared<data::vector>();
    for(const auto& obj : *_current_selection)
    {
        SIGHT_ASSERT("Object not instanced in selection", obj);
        if(obj->is_a(_type))
        {
            sub_selection->push_back(obj);
        }
    }

    return sub_selection;
}

//------------------------------------------------------------------------------

} // namespace sight::activity::builder
