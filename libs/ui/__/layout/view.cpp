/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/__/layout/view.hpp"

namespace sight::ui::layout
{

//-----------------------------------------------------------------------------

void view::destroy_sub_views()
{
    for(const ui::container::widget::sptr& container : m_sub_views)
    {
        container->destroy_container();
    }

    m_sub_views.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::container::widget::sptr> view::get_sub_views()
{
    return this->m_sub_views;
}

//------------------------------------------------------------------------------

void view::modify_layout(const ui::parameter_t& /*parameter*/, const std::string& _key)
{
    SIGHT_THROW(
        "'" + std::string(__func__) + "' is not implemented for this layout manager with the key '" + _key + "'."
    );
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::layout
