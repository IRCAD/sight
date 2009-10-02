/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_IEDITIONSERVICE_HPP_
#define _FWSERVICES_IEDITIONSERVICE_HPP_

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Object.hpp>


#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{
class ObjectMsg ;
class ComChannelService ;
}

namespace fwServices
{

/**
 * @brief	This interface defines edition service API. It mainly has the role of the subject in the observer design pattern.
 * @class	IEditionService.
 * @author IRCAD (Research and Development Team).

 * @date	2007-2009.
 *
 * This service notifies modifications performed on its associated object. This can be seen a delegate of traditional notifications performed by objects (subjects) which are themselves subject to modification. This ensures a good separation of concerns.
 * @todo the design assumes that any object can have only one service of type IEditionService. For robustness, this should be controlled.
 * @note notification are propagated to "listening" services by invoking the update(::boost::shared_ptr< const fwServices::ObjectMsg > ) methods on referenced communication channels which forward this to their destination service acting as the observer.
 * @note could be improved by directly registering the destination service instead of the communication channel. Communication channel, being a first class entity between the subject and observer, have to be preferred to facilitate the configuration, the (des)activation and the monitoring of communications over a software system.
 * @note special thanks to Guillaume Brocker
 */
class FWSERVICES_CLASS_API IEditionService : public fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IEditionService)(::fwServices::IService::Baseclass) ) ;

	/**
	 * @brief Invoke the IEditionService::notify( ::boost::shared_ptr< const ObjectMsg > ) method on _pSubject.
	 * @param[in] _pSource The service which send the message
	 * @param[in] _pSubject The subject of the message ( to retrieve listeners of the subject )
	 * @param[in] _pMsg message notify to associated object listeners
	 *
	 *  This method notifies observers of _pSubject. It changes notification status of _pSource during all notification, set correct source and subject of msg and timestamp the message.
	 */
	FWSERVICES_API static void notify(
			::boost::shared_ptr< ::fwServices::IService > _pSource,
			::boost::shared_ptr< ::fwTools::Object > _pSubject,
			::boost::shared_ptr< ObjectMsg > _pMsg );


	/// Constructor. Do nothing.
	FWSERVICES_API IEditionService() ;

	/// Destructor. Do nothing.
	FWSERVICES_API virtual ~IEditionService() throw();


	/**
	 * @name Methods to manage observations (observer design pattern)
	 */
	//@{


	/**
	 * @brief Attaches the specified observer to the subject.
	 * @param observer a shared pointer to the observer instance to attach
	 * @note The observer is referenced as weak reference in the list of listener
	 */
	FWSERVICES_API void attach( ::boost::shared_ptr< ::fwServices::ComChannelService > observer ) throw();

	/**
	 * @brief Detaches the specified observer from the subject.
	 * @param observer a shared pointer to the observer instance to detach
	 */
	FWSERVICES_API void detach( ::boost::shared_ptr< ::fwServices::ComChannelService > observer ) throw();

	/**
	 * @brief Tells if the given observer is already attached to the subject. From observer library.
	 * @param observer a shared pointer to an observer
	 */
	FWSERVICES_API const bool isAttached( ::boost::shared_ptr< ::fwServices::ComChannelService > observer ) const throw();

	//@}

private :

	/// Defines the observer container type. From observer library.
	typedef std::list< ::boost::weak_ptr< ::fwServices::ComChannelService > > ObserverContainer;

	/// Contains all attached observers. From observer library.
	ObserverContainer m_observers;

	/**
	 * @name Helpers to manage observations
	 */
	//@{

	/**
	 * @brief Retrieves the iterator on the given observer. From observer library.
	 *
	 * @param observer a shared pointer to an observer
	 */
	ObserverContainer::iterator findObserver( ::boost::shared_ptr< fwServices::ComChannelService > observer ) throw();

	/**
	 * @brief Retrieves the iterator on the given observer. From observer library.
	 *
	 * @param observer a shared pointer to an observer
	 */
	ObserverContainer::const_iterator findObserver( ::boost::shared_ptr< fwServices::ComChannelService > observer ) const throw();

	//@}

protected:

	/**
	 * @brief This method forwards an eventMessage to all related observing services (generally through a ICommunication service).
	 * @param[in] eventMessage message forwards at all listeners
	 * @todo Conflict with notify in IService class
	 * @todo In this method, observers expired are also removed, can be optimized.
	 */
	FWSERVICES_API void notify( ::boost::shared_ptr< const ObjectMsg > eventMessage ) ;
};

}

#endif // _FWSERVICES_IEDITIONSERVICE_HPP_
