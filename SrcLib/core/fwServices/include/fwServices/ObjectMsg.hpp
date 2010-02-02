/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_OBJECTMSG_HPP_
#define _FWSERVICES_OBJECTMSG_HPP_

#include <map>
#include <string>
#include <vector>

#include <fwTools/Object.hpp>
#include <fwData/Object.hpp>


#include "fwServices/config.hpp"

namespace fwServices
{

class IService;

/**
 * @brief Object message : indicate modifications having occurred or being performed on a object.
 * @class ObjectMsg
 * @author IRCAD (Research and Development Team).
 * @date 2007-2009.
 *
 * The message only provides identifiers enabling to retrieve the modified information on the object. This should be embed the information. The m_subject is present
 * to check that the considered object (subject to modifications) is the one known (or referenced) by the service receiving the message through the observation mechanism.
 *
 * The m_subject should a priori not be used by a service to access the object. Indeed, to be able to work without observation (therefore without messages, this being possible as observations are optional), a service requires
 * a reference to the object allowing to perform computations. This reference is generally acquired either at attachment time (e.g. the object of interest being the one the considered service is attached to) or at configuration time (e.g. passing an unique universal identifier in the XML configuration).
 * Therefore, for coherence and homogeneity, a service should be able to access object(s) it requires in the same way, whatever the working mode (i.e. explicit execution without observations or implicit execution with observation).
 *
 * Concerning the type of m_subject : a software instance is based on a (root) tree structured complex object (to which services are attached, corresponding to functionalities). So that
 * the root object naturally destroys through unreferencement, it must be avoided that instances (except pure data objects) keep strong references on data objects composing the root, otherwise
 * there is a risk of cross strong references, forgiving the natural destruction of objects. For this reason, messages should also respect this rule and the m_subject should be a weak reference (weak pointer).
 *
 * The ObjectMsg contains event identifiers with associated additional information.
 */
class FWSERVICES_CLASS_API ObjectMsg : public ::fwTools::Object
{

public:

	fwCoreClassDefinitionsWithFactoryMacro( (ObjectMsg)(::fwTools::Object::Baseclass), (()), ::fwTools::Factory::New< ObjectMsg > );

	/// Constructor, do nothing.
	FWSERVICES_API ObjectMsg();

	/// Destructor, do nothing.
	FWSERVICES_API virtual ~ObjectMsg();


	/**
	 * @name Event managment
	 */
	//@{

	/**
	 * @brief This method add an event to the message before message notification.
	 * @param[in] _eventId the event added in the message
	 * @param[in] _pDataInfo additional information concerning message ( it is an optional parameter )
	 * @note a message can contain many events.
	 */
	FWSERVICES_API void addEvent( std::string _eventId , ::fwData::Object::sptr _pDataInfo = ::fwData::Object::sptr() );

	/**
	 * @brief This method is used to know if a specific event declared in the message
	 * @param[in] _eventId id of the event researched
	 * @return true if message contain the event id
	 */
	FWSERVICES_API bool hasEvent( std::string _eventId ) const;

	/**
	 * @brief This method returns all id event contained in the message
	 * @return the list of event
	 */
	FWSERVICES_API std::vector< std::string > getEventIds() const ;

	/**
	 * @brief This method returns the data info associated to event, this data can be an null ptr when the event not needs some additional informations.
	 * @param[in] _eventId event id used to find his associated data
	 * @return associated data information
	 */
	FWSERVICES_API ::fwData::Object::csptr getDataInfo( std::string _eventId ) const;

	//@}


	/**
	 * @name Management of source/subject message
	 */
	//@{

	/// Set message source (service which notifies subject obsevers)
	FWSERVICES_API void setSource( WPTR(::fwServices::IService) _source );

	/// Get message source (service which notifies subject obsevers)
	FWSERVICES_API WPTR(::fwServices::IService) getSource() const;

	/// Set message subject (object observed)
	FWSERVICES_API void setSubject( ::fwTools::Object::wptr _subject );

	/// Get message subject (object observed)
	FWSERVICES_API ::fwTools::Object::wptr getSubject() const;

	//@}


	/// Give some message informations in a string.
	FWSERVICES_API std::string getGeneralInfo() const;

	/**
	 * @brief Streaming a service
	 * @see ObjectMsg::operator<<(std::ostream & _ostream, ObjectMsg& _service)
	 * @note Invoke ObjectMsg::info( std::ostream )
	 */
	FWSERVICES_API friend std::ostream & operator<<(std::ostream & _sstream, const ObjectMsg& _message) ;

protected :

	/**
	 * @brief Intern map which associates an Event Id (string) to a dataInfo ( ::fwData::Object ).
	 *
	 * Some event has not data info, in this cases, the event is in map  but the ::fwData::Object::sptr is null.
	 */
	std::map< std::string , ::fwData::Object::sptr > m_eventId2DataInfo;

	/// Give some message informations, this method uses getGeneralInfo.
	FWSERVICES_API virtual void info(std::ostream &_sstream ) const ;

private :

	/// Message source. It is the service which creates msg and sends it at all subject observers
	WPTR(::fwServices::IService) m_source;

	/// Message subject. All subject observers will be notified.
	::fwTools::Object::wptr m_subject;

	/// Helper to convert string UUID/Classname in pretty string
	static std::string convertToLightString( std::string _initialString );

};


} // namespace fwServices

#endif //_FWSERVICES_OBJECTMSG_HPP_

