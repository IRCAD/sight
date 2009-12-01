/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <cassert>
#include <fwTools/ClassFactoryRegistry.hpp>

#include "fwServices/ComChannelService.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/macros.hpp"
#include "fwServices/ObjectMsg.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/op/Get.hpp"

namespace fwServices
{

namespace
{

//-----------------------------------------------------------------------------

/**
 * @brief	Defines a STL compatible predicate that test if a given weak pointer
 * 			has expired or not.
 */
struct HasExpired
{
	const bool operator() ( ::fwServices::ComChannelService::wptr observer ) const
	{
		return observer.expired();
	}
};

//-----------------------------------------------------------------------------

/**
 * @brief	Defines a predicate STL compatible that test the equality of weak pointers to
 * 			a given shared pointer.
 */
struct IsEqual
{
	IsEqual( ::fwServices::ComChannelService::sptr observer )
	: m_observer( observer )
	{}

	const bool operator() ( ::fwServices::ComChannelService::wptr observer ) const
	{
		return observer.lock() == m_observer;
	}

	private:

		 ::fwServices::ComChannelService::sptr m_observer;
};

//-----------------------------------------------------------------------------

} // end namespace

//-----------------------------------------------------------------------------

void IEditionService::notify(
		::fwServices::IService::sptr _pSource,
		::fwTools::Object::sptr _pSubject,
		::fwServices::ObjectMsg::sptr _pMsg,
		::fwServices::ComChannelService::MsgOptionsType options )
{

	_pSource->sendingModeOn();
	_pMsg->setSource(_pSource);
	_pMsg->setSubject(_pSubject);
	_pMsg->timeModified();
	OSLM_INFO( "MSG Notification : " << _pMsg->getGeneralInfo() );
	::fwServices::get< ::fwServices::IEditionService >( _pSubject )->notify( _pMsg, options ) ;
	_pSource->sendingModeOff();

}

//-----------------------------------------------------------------------------

IEditionService::IEditionService()
{}

//-----------------------------------------------------------------------------

IEditionService::~IEditionService() throw()
{}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
void IEditionService::attach( ::fwServices::ComChannelService::sptr observer) throw()
{
	// Pre-condition
	assert( observer != 0 );

	// If not already registered, stores the referenced observer.
	if( findObserver(observer) == m_observers.end() )
	{
		m_observers.push_back( observer );
	}
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
void IEditionService::detach( ::fwServices::ComChannelService::sptr observer ) throw()
{
	// Removes the desired observer.
	m_observers.remove_if( IsEqual(observer) );
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
const bool IEditionService::isAttached( ::fwServices::ComChannelService::sptr observer ) const throw()
{
	return findObserver( observer ) != m_observers.end();
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
IEditionService::ObserverContainer::iterator IEditionService::findObserver( ::fwServices::ComChannelService::sptr observer ) throw()
{
	return std::find_if( m_observers.begin(), m_observers.end(), IsEqual(observer) );
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
IEditionService::ObserverContainer::const_iterator IEditionService::findObserver( ::fwServices::ComChannelService::sptr observer ) const throw()
{
	return std::find_if( m_observers.begin(), m_observers.end(), IsEqual(observer) );
}

//-----------------------------------------------------------------------------

void IEditionService::notify( ::fwServices::ObjectMsg::csptr eventMessage, ::fwServices::ComChannelService::MsgOptionsType options )
{
	// We work on copy of the observer collection because observers could be attached
	// or detached while forwarding the event and this would invalidate iterators
	// or lead to inconsistent notifications.
	ObserverContainer	observers	( m_observers );
	ObserverContainer::iterator iter ;
	for( iter = observers.begin() ; iter != observers.end() ; ++iter )
	{
		// Expurge expired observers from the main observer collection.
		if( iter->expired() )
		{
			m_observers.remove_if( HasExpired() );
		}
		// Process message
		else
		{
			(*iter).lock()->sendMessage( eventMessage, options ) ;
		}
	}
}

//-----------------------------------------------------------------------------

}
