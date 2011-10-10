/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/fwID.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwServices/IEditionService.hpp>

#include "gui/action/ActionNotifyService.hpp"

namespace gui
{
namespace action
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::fwGui::IActionSrv, ::gui::action::ActionNotifyService , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

ActionNotifyService::ActionNotifyService() throw()
{
    handlingEventOff();
}

//-----------------------------------------------------------------------------

ActionNotifyService::~ActionNotifyService() throw()
{}

//-----------------------------------------------------------------------------

void ActionNotifyService::info(std::ostream &_sstream )
{
    _sstream << "NotifyService Action" << std::endl;
}

//-----------------------------------------------------------------------------

void ActionNotifyService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void ActionNotifyService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void ActionNotifyService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void ActionNotifyService::updating() throw( ::fwTools::Failed )
{
    if (this->confirmAction())
    {
        BOOST_FOREACH(MsgEventType msg, m_vectMsg)
        {
            const std::string msgType = msg.get<0>();
            const std::string event   = msg.get<1>();
            ::fwTools::Object::sptr obj = ::fwTools::Factory::New(msgType);
            OSLM_ASSERT(msgType << " creation failed", obj);
            ::fwServices::ObjectMsg::sptr objectMsg = ::fwServices::ObjectMsg::dynamicCast(obj);
            OSLM_ASSERT(msgType << " dynamicCast failed", objectMsg);

            ::fwTools::Object::sptr srvObj = this->getObject();
            objectMsg->addEvent( event );
            ::fwServices::IEditionService::notify(this->getSptr(), srvObj, objectMsg);
        }
    }
}

//-----------------------------------------------------------------------------

void ActionNotifyService::configuring() throw( ::fwTools::Failed )
{
    this->initialize();

    m_vectMsg.clear();
    std::vector < ::fwRuntime::ConfigurationElement::sptr > notify = m_configuration->find("notify");
    SLM_ASSERT("Problem with configuration for ActionNotifyService, missing element \"notify\"", !notify.empty() );
    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr msgConfig, notify)
    {
        SLM_ASSERT("Missing attribute type", msgConfig->hasAttribute("type"));
        std::string type = msgConfig->getAttributeValue("type");
        SLM_ASSERT("Missing attribute event", msgConfig->hasAttribute("event"));
        std::string event = msgConfig->getAttributeValue("event");
        MsgEventType msg (type, event);
        m_vectMsg.push_back( msg );
    }
}

//-----------------------------------------------------------------------------

}
}
