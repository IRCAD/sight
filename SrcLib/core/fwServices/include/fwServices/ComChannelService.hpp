/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef COMMUNICATIONCHANNELSERVICE_HPP_
#define COMMUNICATIONCHANNELSERVICE_HPP_

#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "fwServices/config.hpp"
#include "fwServices/ICommunication.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{

class ObjectMsg ;

/**
 * @brief Default communication channel between a source (IEditionService, the subject, associated to a given observed object) and a destination (the service receiving and processing modifications fired by the subject)
 * @class ComChannelService
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 * @date	2007-2009.
 */
class FWSERVICES_CLASS_API ComChannelService : public ::fwServices::ICommunication
{
public:

	/// Definitions
	fwCoreServiceClassDefinitionsMacro ( (ComChannelService)(::fwServices::ICommunication::Baseclass) ) ;

	/**
	 * @brief Constructor
	 */
	FWSERVICES_API ComChannelService()  ;

	/**
	 * @brief Destructor
	 */
	FWSERVICES_API virtual ~ComChannelService() ;

	/**
	 * @name	 IService overriden methods
	 */
	//@{

	/**
	 * @brief If configuration is set, both subject (data) and observer (service) uuid are retrieved
	 */
	FWSERVICES_API virtual void configuring() throw( ::fwTools::Failed ) ;

	/**
	 * @brief Trigger attachment (observation becomes active)
	 * @pre the new com channel (src->dest) is unique in application ( hasSimilarObservation() == false )
	 */
	FWSERVICES_API virtual void starting() throw( ::fwTools::Failed);

	/**
	 * @brief Trigger detachment : observation becomes unactive
	 * @author IRCAD (Research and Development Team).
	 * @author Jean-Baptiste Fasquel
	 */
	FWSERVICES_API virtual void stopping() throw( ::fwTools::Failed);

	/**
	 * @brief include activation status
	 */
	FWSERVICES_API virtual void info(std::ostream &_sstream ) ;

	/**
	 * @brief Propagate modification to destination service
	 */
	FWSERVICES_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(fwTools::Failed);

	/**
	 * @brief Does nothing
	 */
	FWSERVICES_API virtual void updating( ) throw( ::fwTools::Failed) {};

	//@}

	/**
	 * @name	 All information related to communication channel configuration
	 */
	//@{

	/**
	 * @brief Defines the object to observe (internally stored with weak reference)
	 */
	FWSERVICES_API void setSrc( ::fwServices::IEditionService::sptr  src);

	/// Defines the target service
	FWSERVICES_API void setDest( ::fwServices::IService::sptr _client);

	/// Returns the target service
	FWSERVICES_API IService::sptr  getDest();

	/// Returns the target service
	FWSERVICES_API ::fwServices::IEditionService::sptr  getSrc();

	/// Return true is both source and destination are valid pointers
	FWSERVICES_API bool isValid();
	//@}

	/**
	 * @brief Send message to destination service if it is started.
	 * @param[in] _msg the message sent to destination service
	 * @pre Destination service must not be in sending mode to receive a message.
	 * If the message is the message sent by destination service, the message is not notified. Else, we have an event loop and an assertion is created.
	 */
	FWSERVICES_API void sendMessage( ::fwServices::ObjectMsg::csptr _msg );

protected:

	/**
	 * @brief Used to test if they have similar observation already defines when the service is started.
	 * @return true if there are similar observations
	 */
	FWSERVICES_API bool hasSimilarObservation() ;

	/**
	 * @brief subject being really observed. It is the editor attached with the object to be modified
	 */
	::fwServices::IEditionService::wptr m_source ;

	/**
	 * client will be invoked on modification
	 * @note required to know the target of an observation
	 */
	::fwServices::IService::wptr m_destination ;

	/// Destination specified by UUID (UUID of the target IService to which modifications are redirected)
	std::pair< bool , std::string > m_destUUID ;

private :

	/// Helper to print pretty message information
	std::string getNotificationInformation( ::fwServices::ObjectMsg::csptr _msg );

	/// Helper to convert string used by ComChannelService::getNotificationInformation
	std::string convertToLightString( std::string _initialString );

};

}

#endif /*COMMUNICATIONCHANNELSERVICE_HPP_*/
