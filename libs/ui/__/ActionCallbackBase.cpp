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

#include "ui/__/ActionCallbackBase.hpp"

#include "ui/__/action.hpp"

#include <core/tools/id.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>
#include <service/op/Get.hpp>

namespace sight::ui
{

//-----------------------------------------------------------------------------

const ActionCallbackBase::RegistryKeyType ActionCallbackBase::REGISTRY_KEY = "::ui::ActionCallback";

//-----------------------------------------------------------------------------

void ActionCallbackBase::setSID(std::string sid)
{
    this->m_sid = sid;
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::execute()
{
    SIGHT_ASSERT("Service " << m_sid << " doesn't exist.", core::tools::id::exist(m_sid));
    service::base::sptr service = service::get(m_sid);
    SIGHT_ASSERT("Service " << m_sid << " not instanced.", service);
    service->update();
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::check(bool checked)
{
    SIGHT_ASSERT("Service " << m_sid << " doesn't exist.", core::tools::id::exist(m_sid));
    service::base::sptr service = service::get(m_sid);
    SIGHT_ASSERT("Service " << m_sid << " not instanced.", service);
    ui::action::sptr action = std::dynamic_pointer_cast<ui::action>(service);
    SIGHT_ASSERT("Service " << m_sid << " is not an action.", action);
    checked = (action->inverted() ? !checked : checked);
    if(action->checked() != checked)
    {
        action->setChecked(checked);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
