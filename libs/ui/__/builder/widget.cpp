/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

/**
 * @file ui/__/builder/widget.cpp
 * @brief This file defines the implementation of the interface class  for the menubar builder.
 */

#include "ui/__/builder/widget.hpp"

namespace sight::ui::builder
{

const widget::registry_key_t widget::REGISTRY_KEY = "::ui::widget";

//-----------------------------------------------------------------------------

widget::widget()
= default;

//-----------------------------------------------------------------------------

widget::~widget()
= default;

//-----------------------------------------------------------------------------

void widget::initialize(const ui::config_t& /*unused*/)
{
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr widget::get_container()
{
    return this->m_container;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::builder
