/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <fwTools/fwID.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include "fwGui/IActionSrv.hpp"
#include "fwGui/ActionCallbackBase.hpp"

#include <fwServices/op/Get.hpp>

namespace fwGui
{

//-----------------------------------------------------------------------------

const ActionCallbackBase::RegistryKeyType ActionCallbackBase::REGISTRY_KEY = "::fwGui::ActionCallback";

//-----------------------------------------------------------------------------

ActionCallbackBase::ActionCallbackBase() : m_sid("")
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
    OSLM_ASSERT("Service "<<m_sid<<" doesn't exist.", ::fwTools::fwID::exist(m_sid ));
    ::fwServices::IService::sptr service = ::fwServices::get( m_sid );
    OSLM_ASSERT("Service "<<m_sid<<" not instanced.", service);
    service->update();
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::check(bool checked)
{
    OSLM_ASSERT("Service "<<m_sid<<" doesn't exist.", ::fwTools::fwID::exist(m_sid ));
    ::fwServices::IService::sptr service = ::fwServices::get( m_sid );
    OSLM_ASSERT("Service "<<m_sid<<" not instanced.", service);
    ::fwGui::IActionSrv::sptr action = ::fwGui::IActionSrv::dynamicCast(service);
    OSLM_ASSERT("Service "<<m_sid<<" is not an action.", action);
    checked = (checked == action->getActiveStateValue());
    if (action->getIsActive() != checked)
    {
        action->setIsActive(checked);
    }
}

//-----------------------------------------------------------------------------


} // namespace fwGui


