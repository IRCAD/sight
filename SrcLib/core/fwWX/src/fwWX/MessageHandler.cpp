/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/GlobalEventManager.hpp>

#include "fwWX/MessageEvent.hpp"
#include "fwWX/MessageHandler.hpp"


REGISTER_SERVICE( ::fwServices::IDeliveryDelegate , ::fwWX::MessageHandler , ::fwTools::Object ) ;

namespace fwWX
{

MessageHandler::MessageHandler() throw()
{
    SLM_TRACE_FUNC();
    m_msgHandler = ::fwServices::GlobalEventManager::getDefault();
}

MessageHandler::~MessageHandler() throw()
{
    SLM_TRACE_FUNC();
}

void MessageHandler::configuring() throw( ::fwTools::Failed ) 
{
}

void MessageHandler::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    m_msgHandler->setNotifyHandler(&MessageHandler::addNewMessageToWxQueue);

    m_onMessageHandler = ::boost::bind( &MessageHandler::onMessage, this, _1 );
    wxTheApp->Bind( MESSAGE_EVENT, m_onMessageHandler ); 

    m_oldDeliveryType = m_msgHandler->getDeliveryType();
    m_msgHandler->setDeliveryType(::fwServices::GlobalEventManager::DELEGATED_BREADTH_FIRST);
}

void MessageHandler::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    m_msgHandler->setDeliveryType(m_oldDeliveryType);
    wxTheApp->Unbind( MESSAGE_EVENT, m_onMessageHandler ); 
    m_msgHandler->setNotifyHandler(NULL);
}

void MessageHandler::updating() throw( ::fwTools::Failed ) 
{
}

void MessageHandler::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
}

void MessageHandler::info( std::ostream &_sstream ) 
{
}




void MessageHandler::addNewMessageToWxQueue()
{
    ::fwWX::MessageEvent msgevt(MESSAGE_EVENT);
    wxTheApp->AddPendingEvent( msgevt );
    //wxTheApp->QueueEvent( msgevt.Clone() );
}


void MessageHandler::onMessage(::fwWX::MessageEvent &event)
{
    if (m_msgHandler->pending())
    {
        m_msgHandler->dispatch();
    }
    event.Skip(true);
    event.StopPropagation();
}

}

