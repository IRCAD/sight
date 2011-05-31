/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>

#include "fwGui/IActionSrv.hpp"
#include "fwGui/ActionCallbackBase.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

const ActionCallbackBase::RegistryKeyType ActionCallbackBase::REGISTRY_KEY =  "::fwGui::ActionCallback";

//-----------------------------------------------------------------------------

ActionCallbackBase::ActionCallbackBase(): m_sid("")
{}

//-----------------------------------------------------------------------------

ActionCallbackBase::~ActionCallbackBase()
{}

//-----------------------------------------------------------------------------

void ActionCallbackBase::setSID(std::string sid)
{
    this->m_sid = sid;
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::execute()
{
    OSLM_ASSERT("Service "<<m_sid<<" doesn't exist.", ::fwTools::fwID::exist(m_sid ));
    ::fwServices::IService::sptr service = ::fwServices::get( m_sid ) ;
    OSLM_ASSERT("Service "<<m_sid<<" not instanced.", service);
    service->update();
}

//-----------------------------------------------------------------------------

void ActionCallbackBase::check(bool checked)
{
    OSLM_ASSERT("Service "<<m_sid<<" doesn't exist.", ::fwTools::fwID::exist(m_sid ));
    ::fwServices::IService::sptr service = ::fwServices::get( m_sid ) ;
    OSLM_ASSERT("Service "<<m_sid<<" not instanced.", service);
    ::fwGui::IActionSrv::sptr action = ::fwGui::IActionSrv::dynamicCast(service);
    OSLM_ASSERT("Service "<<m_sid<<" is not an action.", action);
    if (action->getIsActive() != checked)
    {
        action->setIsActive(checked);
    }
}

//-----------------------------------------------------------------------------


} // namespace fwGui


