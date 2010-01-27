/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_GLOBALEVENTMANAGER_HPP_
#define _FWSERVICES_GLOBALEVENTMANAGER_HPP_

#include <fwCore/base.hpp>
#include <fwTools/Singleton.hpp>

#include "fwServices/config.hpp"
#include "fwServices/ObjectMsg.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @brief	Event Manager of the application
 *
 * @class	GlobalEventManager
 * @author  IRCAD (Research and Development Team).
 * @date	2010
 */
class FWSERVICES_CLASS_API GlobalEventManager
{


public:

	enum DeliveryType {
		BREADTH_FIRST,
		DEPTH_FIRST,
		DELEGATED_BREADTH_FIRST
	};

	FWSERVICES_API bool pending();
	FWSERVICES_API void dispatch();

	FWSERVICES_API static SPTR(GlobalEventManager) getDefault()
	{
		if ( !m_ClassInstance )
		{
			m_ClassInstance = SPTR(GlobalEventManager) (new GlobalEventManager);
		}
		return m_ClassInstance;
	}

	FWSERVICES_API void notify( ::fwServices::ObjectMsg::sptr _pMsg, ::fwServices::ComChannelService::MsgOptionsType _options );

	
	void setNotifyHandler(void (*handler)()) 
	{
		m_notifyHandler = handler;
	};

	virtual ~GlobalEventManager();
protected :

	GlobalEventManager();

	FWSERVICES_API static SPTR(GlobalEventManager) m_ClassInstance;

	//	bool pushEventInDeque( ::fwServices::ObjectMsg::sptr _pMsg, ::fwServices::ComChannelService::MsgOptionsType _options );
	//
	//	bool messagesHaveSameEffect( ::fwServices::ObjectMsg::sptr _pMsg1, ::fwServices::ObjectMsg::sptr _pMsg2 );

	typedef std::pair< ::fwServices::ObjectMsg::sptr, ::fwServices::ComChannelService::MsgOptionsType > MessageAndOptions;

	std::deque< MessageAndOptions > m_msgDeque;

	DeliveryType m_deliveryType;
	bool        m_dispatching;

	void (*m_notifyHandler)();

};

}

#endif // _FWSERVICES_GLOBALEVENTMANAGER_HPP_
