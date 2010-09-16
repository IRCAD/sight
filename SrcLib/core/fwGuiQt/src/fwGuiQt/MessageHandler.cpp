/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>

#include <QApplication>
#include <QEvent>

#include <fwServices/Base.hpp>
#include <fwServices/GlobalEventManager.hpp>

#include "fwGuiQt/MessageHandler.hpp"


REGISTER_SERVICE( ::fwServices::IDeliveryDelegate , ::fwGui::MessageHandler , ::fwTools::Object ) ;

namespace fwGui
{

int MessageHandler::s_qtMessageHandlerEventType = QEvent::registerEventType();

//------------------------------------------------------------------------------

MessageHandler::MessageHandler() throw()
{
    SLM_TRACE_FUNC();

    m_msgHandler = ::fwServices::GlobalEventManager::getDefault();
}

//------------------------------------------------------------------------------

MessageHandler::~MessageHandler() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void MessageHandler::configuring() throw( ::fwTools::Failed )
{}

//------------------------------------------------------------------------------

void MessageHandler::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::boost::function0< void > notifyHandler;
    notifyHandler = ::boost::bind( &MessageHandler::addNewMessageToQtQueue, this );
    m_msgHandler->setNotifyHandler(notifyHandler);

    m_oldDeliveryType = m_msgHandler->getDeliveryType();
    m_msgHandler->setDeliveryType(::fwServices::GlobalEventManager::DELEGATED_BREADTH_FIRST);
}

//------------------------------------------------------------------------------

void MessageHandler::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    m_msgHandler->setDeliveryType(m_oldDeliveryType);
    m_msgHandler->setNotifyHandler(NULL);
}

//------------------------------------------------------------------------------

void MessageHandler::updating() throw( ::fwTools::Failed )
{}

//------------------------------------------------------------------------------

void MessageHandler::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{}

//------------------------------------------------------------------------------

void MessageHandler::info( std::ostream &_sstream )
{}

//------------------------------------------------------------------------------

void MessageHandler::addNewMessageToQtQueue()
{
    qApp->postEvent( this, new QEvent((QEvent::Type) s_qtMessageHandlerEventType) );
}

//------------------------------------------------------------------------------


bool MessageHandler::event ( QEvent * e )
{
    bool bRes = false;
    if (e->type() == s_qtMessageHandlerEventType)
    {
        if (m_msgHandler->pending())
        {
            m_msgHandler->dispatch();
        }
        e->accept();
        bRes = true;
    }
    return bRes;
}

//------------------------------------------------------------------------------

}


