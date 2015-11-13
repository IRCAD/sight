/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/ActionNotifyService.hpp"

#include <fwCore/base.hpp>

#include <fwTools/fwID.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>


#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

namespace gui
{
namespace action
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::ActionNotifyService, ::fwData::Object );

//-----------------------------------------------------------------------------

ActionNotifyService::ActionNotifyService() throw()
{
    //handlingEventOff();
}

//-----------------------------------------------------------------------------

ActionNotifyService::~ActionNotifyService() throw()
{
}

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

void ActionNotifyService::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void ActionNotifyService::updating() throw( ::fwTools::Failed )
{
    if (this->confirmAction())
    {
        for(MsgEventType msg :  m_vectMsg)
        {
            const std::string msgType = msg.get<0>();
            const std::string event   = msg.get<1>();
            ::fwServices::ObjectMsg::sptr objectMsg = ::fwServices::factory::message::New(msgType);
            OSLM_ASSERT(msgType << " creation failed", objectMsg);

            ::fwData::Object::sptr srvObj = this->getObject();
            objectMsg->addEvent( event );
            objectMsg->setSource(this->getSptr());
            objectMsg->setSubject( srvObj);
            ::fwData::Object::ObjectModifiedSignalType::sptr sig;
            sig = srvObj->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
                sig->asyncEmit( objectMsg);
            }
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
    for(::fwRuntime::ConfigurationElement::sptr msgConfig :  notify)
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
