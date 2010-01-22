#include "fwServices/GlobalEventManager.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/op/Get.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

::boost::shared_ptr< GlobalEventManager > GlobalEventManager::m_ClassInstance;

//-----------------------------------------------------------------------------

GlobalEventManager::GlobalEventManager() :
	m_analyseType ( BREADTH_FIRST_ANALYSE )
//	m_analyseType ( DEPTH_FIRST_ANALYSE )
{}

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

void GlobalEventManager::notify( ::fwServices::ObjectMsg::sptr _pMsg, ::fwServices::ComChannelService::MsgOptionsType _options )
{

	if ( m_analyseType == BREADTH_FIRST_ANALYSE )
	{
		OSLM_INFO( "MSG Register Notification : " << _pMsg->getGeneralInfo() );
		MessageAndOptions msg ( _pMsg, _options );
		m_msgDeque.push_back( msg );
		//pushEventInDeque( _pMsg, _options );

		if ( m_msgDeque.size() == 1 )
		{
			while ( ! m_msgDeque.empty() )
			{
				MessageAndOptions msgAndOpt = m_msgDeque.front();

				::fwServices::ObjectMsg::sptr pMsg = msgAndOpt.first;
				::fwServices::ComChannelService::MsgOptionsType options = msgAndOpt.second;
				::fwServices::IService::sptr pSource = pMsg->getSource().lock();
				::fwTools::Object::sptr pSubject = pMsg->getSubject().lock();

				OSLM_INFO( "MSG Apply Notification : " << pMsg->getGeneralInfo() );
				::fwServices::IEditionService::sptr srv;
				srv = ::fwServices::get< ::fwServices::IEditionService >( pSubject );
				srv->notify( pMsg, options ) ;

				m_msgDeque.pop_front();
			}
		}
	}
	else // if ( m_analyseType == DEPTH_FIRST_ANALYSE )
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
