/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>

#include <fwServices/GlobalEventManager.hpp>

#include "fwWX/MessageEvent.hpp"
#include "fwWX/MessageHandler.hpp"


namespace fwWX
{

MessageHandler::MessageHandler() throw()
{
	SLM_TRACE_FUNC();
	m_msgHandler = ::fwServices::GlobalEventManager::getDefault();
	m_msgHandler->setNotifyHandler(&MessageHandler::addNewMessageToWxQueue);
	wxTheApp->Connect( MESSAGE_EVENT, (wxObjectEventFunction) & MessageHandler::OnMessage, 0, this);
}

MessageHandler::~MessageHandler() throw()
{
	SLM_TRACE_FUNC();
}

void MessageHandler::addNewMessageToWxQueue()
{
	::fwWX::MessageEvent msgevt(MESSAGE_EVENT);
	wxTheApp->AddPendingEvent( msgevt );
	//wxTheApp->QueueEvent( msgevt.Clone() );
}


void MessageHandler::OnMessage(::fwWX::MessageEvent &event)
{
	if (m_msgHandler->pending())
	{
		m_msgHandler->dispatch();
	}
	event.Skip(true);
	event.StopPropagation();
}

}

