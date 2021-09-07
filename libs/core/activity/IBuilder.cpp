/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "activity/IBuilder.hpp"

#include <data/Vector.hpp>

namespace sight::activity
{

//------------------------------------------------------------------------------

data::Vector::sptr IBuilder::getType(
    const data::Vector::csptr& currentSelection,
    const std::string& type
) const
{
    SIGHT_ASSERT("currentSelection not instanced", currentSelection);

    data::Vector::sptr subSelection = data::Vector::New();
    for(data::Vector::value_type obj : *currentSelection)
    {
        SIGHT_ASSERT("Object not instanced in selection", obj);
        if(obj->isA(type))
        {
            subSelection->getContainer().push_back(obj);
        }
    }

    return subSelection;
}

//------------------------------------------------------------------------------

} // namespace sight::activity
