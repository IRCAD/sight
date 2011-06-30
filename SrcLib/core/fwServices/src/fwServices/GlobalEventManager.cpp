/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/GlobalEventManager.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/op/Get.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

SPTR( GlobalEventManager ) GlobalEventManager::m_ClassInstance;

//-----------------------------------------------------------------------------
SPTR( GlobalEventManager ) GlobalEventManager::getDefault()
{
    if ( !m_ClassInstance )
    {
        m_ClassInstance = SPTR(GlobalEventManager) (new GlobalEventManager);
    }
    return m_ClassInstance;
}

//-----------------------------------------------------------------------------

GlobalEventManager::GlobalEventManager() :
    m_deliveryType ( BREADTH_FIRST ),
    //m_deliveryType ( DELEGATED_BREADTH_FIRST ),
    //m_deliveryType ( DEPTH_FIRST ),
    m_dispatching ( false ),
    m_notifyHandler(NULL)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

GlobalEventManager::~GlobalEventManager()
{
    SLM_TRACE_FUNC();
    this->clearMessages();
}

//-----------------------------------------------------------------------------

void GlobalEventManager::clearMessages()
{
    m_msgDeque.clear();
}

//-----------------------------------------------------------------------------

bool GlobalEventManager::pending()
{
    return !m_msgDeque.empty();
}

//-----------------------------------------------------------------------------

void GlobalEventManager::dispatch()
{
    SLM_ASSERT( "Cannot re-enter in GlobalEventManager::dispatch()",  !m_dispatching  );
    SLM_ASSERT( "Message queue is empty", !m_msgDeque.empty() );
    m_dispatching = true;

    MessageAndOptions msgAndOpt = m_msgDeque.front();
    ::fwServices::ObjectMsg::sptr                   pMsg     = msgAndOpt.first;
    ::fwServices::ComChannelService::MsgOptionsType options  = msgAndOpt.second;

    SLM_WARN_IF( "Message's subject expired", pMsg->getSubject().expired() );
    if(!pMsg->getSubject().expired())
    {
        SLM_INFO_IF( "Message's source expired", pMsg->getSource().expired());
        OSLM_INFO( "dispatching MSG : " << pMsg->getGeneralInfo() );
        ::fwTools::Object::sptr pSubject = pMsg->getSubject().lock();
        if( ::fwServices::OSR::has(pSubject, "::fwServices::IEditionService") )
        {
            ::fwServices::IEditionService::sptr srv;
            srv = ::fwServices::get< ::fwServices::IEditionService >( pSubject );
            srv->notify( pMsg, options ) ;
            srv->getRootedClassname();
        }
    }
    m_msgDeque.pop_front();
    m_dispatching = false;
}

//-----------------------------------------------------------------------------

void GlobalEventManager::notify( ::fwServices::ObjectMsg::sptr _pMsg, ::fwServices::ComChannelService::MsgOptionsType _options )
{

    if ( m_deliveryType == DELEGATED_BREADTH_FIRST )
    {
        OSLM_INFO( "MSG queued : " << _pMsg->getGeneralInfo() );
        MessageAndOptions msg ( _pMsg, _options );
        m_msgDeque.push_back( msg );

        if (m_notifyHandler)
        {
            m_notifyHandler();
        }
    }
    else if ( m_deliveryType == BREADTH_FIRST )
    {
        OSLM_INFO( "MSG queued for immediate notification : " << _pMsg->getGeneralInfo() );
        MessageAndOptions msg ( _pMsg, _options );
        m_msgDeque.push_back( msg );

        if ( m_msgDeque.size() == 1 )
        {
            while ( GlobalEventManager::pending() )
            {
                GlobalEventManager::dispatch();
            }
        }
    }
    else if ( m_deliveryType == DEPTH_FIRST )
    {
        ::fwServices::IService::sptr pSource = _pMsg->getSource().lock();
        ::fwTools::Object::sptr pSubject = _pMsg->getSubject().lock();

        OSLM_INFO( "MSG Notification : " << _pMsg->getGeneralInfo() );
        ::fwServices::IEditionService::sptr srv;
        srv = ::fwServices::get< ::fwServices::IEditionService >( pSubject );
        srv->notify( _pMsg, _options ) ;
    }

}

//-----------------------------------------------------------------------------

} // end namespace fwServices
