#include "fwServices/GlobalEventManager.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/op/Get.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

SPTR( GlobalEventManager ) GlobalEventManager::m_ClassInstance;

//-----------------------------------------------------------------------------

GlobalEventManager::GlobalEventManager() :
    //m_deliveryType ( BREADTH_FIRST ),
    m_deliveryType ( DELEGATED_BREADTH_FIRST ),
    //m_deliveryType ( DEPTH_FIRST ),
    m_dispatching ( false )
{
    SLM_TRACE_FUNC();
}


GlobalEventManager::~GlobalEventManager()
{
    SLM_TRACE_FUNC();
}


//-----------------------------------------------------------------------------

////-----------------------------------------------------------------------------
//
//bool GlobalEventManager::messagesHaveSameEffect( ::fwServices::ObjectMsg::sptr _pMsg1, ::fwServices::ObjectMsg::sptr _pMsg2 )
//{
//	bool messagesHaveSameEffect = ( _pMsg1->getSubject().lock().get() == _pMsg2->getSubject().lock().get() );
//
//	std::vector< std::string > events1 = _pMsg1->getEventIds();
//	std::vector< std::string > events2 = _pMsg2->getEventIds();
//
//	messagesHaveSameEffect =
//			messagesHaveSameEffect &&
//			events1.size() == 1 &&
//			events2.size() == 1 &&
//			events1[0] == events2[0];
//
//	return messagesHaveSameEffect;
//}
//
////-----------------------------------------------------------------------------
//
//bool GlobalEventManager::pushEventInDeque( ::fwServices::ObjectMsg::sptr _pMsg, ::fwServices::ComChannelService::MsgOptionsType _options )
//{
//	if ( m_msgDeque.size() > 1 )
//	{
//		::fwServices::ObjectMsg::sptr pLastMsg = m_msgDeque.back().first;
//		if ( messagesHaveSameEffect( pLastMsg, _pMsg ) )
//		{
//			OSLM_INFO( "MSG Not Register (Same Msg In Deque) : " << _pMsg->getGeneralInfo() );
//		}
//		else
//		{
//			OSLM_INFO( "MSG Register Notification : " << _pMsg->getGeneralInfo() );
//			MessageAndOptions msg ( _pMsg, _options );
//			m_msgDeque.push_back( msg );
//		}
//	}
//	else
//	{
//		OSLM_INFO( "MSG Register Notification : " << _pMsg->getGeneralInfo() );
//		MessageAndOptions msg ( _pMsg, _options );
//		m_msgDeque.push_back( msg );
//	}
//}

//-----------------------------------------------------------------------------


bool GlobalEventManager::pending()
{
    return !m_msgDeque.empty();
}

void GlobalEventManager::dispatch()
{
    SLM_ASSERT( "Cannot re-enter in GlobalEventManager::dispatch()",  !m_dispatching  );
    SLM_ASSERT( "Message queue is empty", !m_msgDeque.empty() );
    m_dispatching = true;

    MessageAndOptions msgAndOpt = m_msgDeque.front();
    ::fwServices::ObjectMsg::sptr                   pMsg     = msgAndOpt.first;
    ::fwServices::ComChannelService::MsgOptionsType options  = msgAndOpt.second;

    SLM_INFO_IF( "Message's sosject expired", ! pMsg->getSubject().expired() );
    OSLM_INFO( "dispatching MSG : " << pMsg->getGeneralInfo() );

    ::fwServices::IService::sptr                    pSource  = pMsg->getSource().lock();
    ::fwTools::Object::sptr                         pSubject = pMsg->getSubject().lock();

    ::fwServices::IEditionService::sptr srv;
    srv = ::fwServices::get< ::fwServices::IEditionService >( pSubject );
    srv->notify( pMsg, options ) ;

    m_msgDeque.pop_front();
    m_dispatching = false;
}

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
		//pushEventInDeque( _pMsg, _options );

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
