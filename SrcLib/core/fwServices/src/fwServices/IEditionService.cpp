/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cassert>
#include <set>

#include <fwTools/ClassFactoryRegistry.hpp>

#include "fwServices/ComChannelService.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/macros.hpp"
#include "fwServices/ObjectMsg.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/op/Com.hpp"
#include "fwServices/op/Get.hpp"

#include "fwServices/GlobalEventManager.hpp"

namespace fwServices
{

namespace
{

//-----------------------------------------------------------------------------

/**
 * @brief   Defines a STL compatible predicate that test if a given weak pointer
 *          has expired or not.
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
 * @brief   Defines a predicate STL compatible that test the equality of weak pointers to
 *          a given shared pointer.
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

/**
 * @brief   Defines a predicate STL compatible that test priority order between two Com Channel Service.
 */

bool Less (::fwServices::ComChannelService::wptr observer1,  ::fwServices::ComChannelService::wptr observer2 )
{
    bool result = false;
    if(!observer1.expired())
    {
        if(!observer2.expired())
        {
            // Both wpstr are valid
            result =
                    ( observer1.lock()->getPriority() > observer2.lock()->getPriority() ) ||
                    ( observer1.lock()->getPriority() == observer2.lock()->getPriority() && observer1.lock().get() > observer2.lock().get() );
        }
        else
        {
            // Normally it never happens because expired comchannel is remove on the IEditionService::notify function
            SLM_FATAL("comChannel expired.");
            // observer1 is valid and obseerver2 is expire.
            result = true;
        }
    }
    else
    {
        if(!observer2.expired())
        {
            // Normally it never happens because expired comchannel is remove on the IEditionService::notify function
            SLM_FATAL("comChannel expired.");
            // Both observer1 and observer1 are expired.
            result = false;
        }
    }
    return result;
}

//-----------------------------------------------------------------------------

/**
 * @brief   Defines a predicate STL compatible that test priority order between two Com Channel Service in a set container.
 */
struct SetLess
{
    bool operator()(::fwServices::ComChannelService::wptr observer1,  ::fwServices::ComChannelService::wptr observer2 )
    {
        return Less(observer1,observer2);
    }
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

    GlobalEventManager::getDefault()->notify( _pMsg, options );
    // OSLM_INFO( "MSG Notification : " << _pMsg->getGeneralInfo() );
    // ::fwServices::IEditionService::sptr srv;
    // srv = ::fwServices::get< ::fwServices::IEditionService >( _pSubject );
    // srv->notify( _pMsg, options ) ;

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
    // Pre-condition : This observer must exist and must not be already registered.
    SLM_ASSERT("The ComChannelService given in parameter is a null shared pointer.", observer != 0 );
    SLM_ASSERT("ACH : The ComChannelService, given in parameter, is already registered", !isAttached( observer ) );

    // Get the service observer
    IService::sptr service = observer->getDest();

    // Test if this service handling all events or if it has a specific configuration
    if ( service->isHandlingAllEvents() )
    {
        OSLM_WARN("Sorry, IService API to manage events has changed, this service "<< service->getClassname() <<" must define a list of handling message thanks to method IService::addNewHandledEvent() which must be used the service constructor.");
        m_globalObservers.push_back( observer );
        // sort the list to put the new element at the right place.
        m_globalObservers.sort( Less );
    }
    else
    {
        OSLM_DEBUG("This service not handling all events : " << service->getClassname() );
        std::vector< std::string > handlingEvents = service->getHandledEvents();
        for (   std::vector< std::string >::iterator iter = handlingEvents.begin();
                iter != handlingEvents.end();
                ++iter )
        {
            std::string handlingEvent = *iter;
            ObserverContainer & specificObservers = m_event2SpecificObservers[ handlingEvent ];
            specificObservers.push_back( observer );
            specificObservers.sort( Less );
        }
    }

    /* OLD METHOD

    // Pre-condition
    assert( observer != 0 );

    // If not already registered, stores the referenced observer.
    if( findObserver(observer) == m_observers.end() )
    {
        m_observers.push_back( observer );
        // sort the list to put the new element at the right place.
        m_observers.sort(Less);
    }

     */
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
void IEditionService::detach( ::fwServices::ComChannelService::sptr observer ) throw()
{
    // Post-condition
    SLM_ASSERT("ACH : The ComChannelService, given in parameter, is not attached", isAttached( observer ) );

    // Get the service observer
    IService::sptr service = observer->getDest();

    // Test if this service handling all events or if it has a specific configuration
    if ( service->isHandlingAllEvents() )
    {
        m_globalObservers.remove_if( IsEqual(observer) );
    }
    else
    {
        std::vector< std::string > handlingEvents = service->getHandledEvents();
        for (   std::vector< std::string >::iterator iter = handlingEvents.begin();
                iter != handlingEvents.end();
                ++iter )
        {
            std::string handlingEvent = *iter;
            ObserverContainer & specificObservers = m_event2SpecificObservers[ handlingEvent ];
            specificObservers.remove_if( IsEqual(observer) );
        }
    }


    /* OLD METHOD

    // Removes the desired observer.
    m_observers.remove_if( IsEqual(observer) );

     */
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
const bool IEditionService::isAttached( ::fwServices::ComChannelService::sptr observer ) const throw()
{
    bool isAttached = false;

    // Get the service observer
    IService::sptr service = observer->getDest();

    // Test if this service handling all events or if it has a specific configuration
    if ( service->isHandlingAllEvents() )
    {
        isAttached = findObserver( m_globalObservers, observer ) != m_globalObservers.end();
    }
    else
    {
        std::vector< std::string > handlingEvents = service->getHandledEvents();
        for (   std::vector< std::string >::iterator iter = handlingEvents.begin();
                !isAttached && iter != handlingEvents.end();
                ++iter )
        {
            std::string handlingEvent = *iter;
            Event2ObserversContainer::const_iterator itOnSpecificObservers = m_event2SpecificObservers.find( handlingEvent );
            if ( itOnSpecificObservers != m_event2SpecificObservers.end() )
            {
                isAttached = findObserver( itOnSpecificObservers->second, observer ) != itOnSpecificObservers->second.end();
            }
            else
            {
                isAttached = false;
            }
        }
    }

    return isAttached;

    /*
    bool isAttached = false;
    isAttached = findObserver( m_globalObservers, observer ) != m_globalObservers.end();

    for (   Event2ObserversContainer::iterator iter = m_event2SpecificObservers.begin();
            !isAttached && iter != m_event2SpecificObservers.end();
            ++iter )
    {
        ObserverContainer specificObservers = iter->second;
        isAttached =  findObserver( specificObservers, observer ) != specificObservers.end();
    }

    return isAttached;
     */
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
IEditionService::ObserverContainer::iterator IEditionService::findObserver( ObserverContainer & _observers, ::fwServices::ComChannelService::sptr observer ) throw()
{
    return std::find_if( _observers.begin(), _observers.end(), IsEqual(observer) );
}

//-----------------------------------------------------------------------------

//@todo remove throw specifications
IEditionService::ObserverContainer::const_iterator IEditionService::findObserver( const ObserverContainer & _observers, ::fwServices::ComChannelService::sptr observer ) const throw()
{
    return std::find_if( _observers.begin(), _observers.end(), IsEqual(observer) );
}

//-----------------------------------------------------------------------------

bool IEditionService::hasExpiredObserver()
{
    bool hasExpiredObserver = false;

    ObserverContainer::const_iterator iterOnObserver;

    iterOnObserver = std::find_if( m_globalObservers.begin(), m_globalObservers.end(), HasExpired() );

    if ( iterOnObserver != m_globalObservers.end() )
    {
        hasExpiredObserver = true;
    }
    else
    {
        for (   Event2ObserversContainer::iterator iter = m_event2SpecificObservers.begin();
                !hasExpiredObserver && iter != m_event2SpecificObservers.end();
                ++iter )
        {
            ObserverContainer & specificObservers = iter->second;
            iterOnObserver = std::find_if( specificObservers.begin(), specificObservers.end(), HasExpired() );
            if ( iterOnObserver != specificObservers.end() )
            {
                hasExpiredObserver = true;
            }
        }
    }

    return hasExpiredObserver;
}

//-----------------------------------------------------------------------------

void IEditionService::notify( ::fwServices::ObjectMsg::csptr eventMessage, ::fwServices::ComChannelService::MsgOptionsType options )
{
    SLM_ASSERT( "ACH : we have expired observer in the observer list", !hasExpiredObserver() );

    typedef std::set< ::fwServices::ComChannelService::wptr, SetLess > NotifiedOberverContainer;
    NotifiedOberverContainer notifiedObservers;

    // Insert global observers
    OSLM_DEBUG( "m_globalObservers.size() == " << m_globalObservers.size() );
    for(    ObserverContainer::iterator iter = m_globalObservers.begin() ;
            iter != m_globalObservers.end() ;
            ++iter )
    {
        notifiedObservers.insert( *iter );
    }


    std::vector< std::string > eventIds = eventMessage->getEventIds();
    for (   std::vector< std::string >::iterator iter = eventIds.begin();
            iter != eventIds.end();
            ++iter )
    {
        std::string eventId = *iter;
        Event2ObserversContainer::iterator itOnSpecificObservers = m_event2SpecificObservers.find( eventId );
        if ( itOnSpecificObservers != m_event2SpecificObservers.end() )
        {
            ObserverContainer & specificObservers = itOnSpecificObservers->second;
            for(    ObserverContainer::iterator iter = specificObservers.begin();
                    iter != specificObservers.end() ;
                    ++iter )
            {
                notifiedObservers.insert( *iter );
            }
        }
    }

    // Notifies messages
    // We work on copy of the observer collection because observers could be attached
    // or detached while forwarding the event and this would invalidate iterators
    // or lead to inconsistent notifications.
    OSLM_DEBUG( "notifiedObservers.size() == " << notifiedObservers.size() );
    for(    NotifiedOberverContainer::iterator iter = notifiedObservers.begin();
            iter != notifiedObservers.end();
            ++iter )
    {
        if( ! iter->expired() )
        {
            (*iter).lock()->sendMessage( eventMessage, options ) ;
        }
    }
}

//-----------------------------------------------------------------------------

}
