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

#include "fwGui/ActionCallbackBase.hpp"

#include "fwGui/IActionSrv.hpp"

#include <core/tools/fwID.hpp>

#include <services/IService.hpp>
#include <services/macros.hpp>
#include <services/op/Get.hpp>

namespace fwGui
{

//-----------------------------------------------------------------------------

const ActionCallbackBase::RegistryKeyType ActionCallbackBase::REGISTRY_KEY = "::fwGui::ActionCallback";

//-----------------------------------------------------------------------------

ActionCallbackBase::ActionCallbackBase()
{
}

//-----------------------------------------------------------------------------

ActionCallbackBase::~ActionCallbackBase()
{
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::setSID(std::string sid)
{
    this->m_sid = sid;
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::execute()
{
    SLM_ASSERT("Service "<<m_sid<<" doesn't exist.", core::tools::fwID::exist(m_sid ));
    services::IService::sptr service = services::get( m_sid );
    SLM_ASSERT("Service "<<m_sid<<" not instanced.", service);
    service->update();
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::check(bool checked)
{
    SLM_ASSERT("Service "<<m_sid<<" doesn't exist.", core::tools::fwID::exist(m_sid ));
    services::IService::sptr service = services::get( m_sid );
    SLM_ASSERT("Service "<<m_sid<<" not instanced.", service);
    ::fwGui::IActionSrv::sptr action = ::fwGui::IActionSrv::dynamicCast(service);
    SLM_ASSERT("Service "<<m_sid<<" is not an action.", action);
    checked = (action->isInverted() ? !checked : checked);
    if (action->getIsActive() != checked)
    {
        action->setIsActive(checked);
    }
}

//-----------------------------------------------------------------------------

} // namespace fwGui
