/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "ui/__/action_callback_base.hpp"

#include "ui/__/action.hpp"

#include <core/id.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>
#include <service/op.hpp>

namespace sight::ui
{

//-----------------------------------------------------------------------------

const action_callback_base::registry_key_t action_callback_base::REGISTRY_KEY = "::ui::action_callback";

//-----------------------------------------------------------------------------

void action_callback_base::set_sid(std::string _sid)
{
    this->m_sid = _sid;
}

//-----------------------------------------------------------------------------

void action_callback_base::execute()
{
    SIGHT_ASSERT("Service " << m_sid << " doesn't exist.", core::id::exist(m_sid));
    service::base::sptr service = service::get(m_sid);
    SIGHT_ASSERT("Service " << m_sid << " not instanced.", service);
    service->update();
}

//-----------------------------------------------------------------------------

void action_callback_base::check(bool _checked)
{
    SIGHT_ASSERT("Service " << m_sid << " doesn't exist.", core::id::exist(m_sid));
    service::base::sptr service = service::get(m_sid);
    SIGHT_ASSERT("Service " << m_sid << " not instanced.", service);
    ui::action::sptr action = std::dynamic_pointer_cast<ui::action>(service);
    SIGHT_ASSERT("Service " << m_sid << " is not an action.", action);
    _checked = (action->inverted() ? !_checked : _checked);
    if(action->checked() != _checked)
    {
        action->set_checked(_checked);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
