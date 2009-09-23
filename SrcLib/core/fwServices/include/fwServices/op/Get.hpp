/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_GET_HPP_
#define _FWSERVICES_GET_HPP_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @name Methods for accessing services
 */
//@{

/**
 * @brief Return a container of all services of type serviceId registered in the system (i.e. attached to the different objects of the system)
 * @note Should be optimized
 * @note Invoke getServices( ::fwTools::Object::sptr , std::string ) for each registered object
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API std::vector< ::fwServices::IService::sptr > getServices( std::string serviceId ) ;

/**
 * @brief Return a container of services of type serviceId which are attached to obj
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API std::vector< ::fwServices::IService::sptr > getServices( ::fwTools::Object::sptr obj , std::string serviceId ) ;

/**
 * @brief Invoke the getServices( ::fwTools::Object::sptr obj , std::string serviceId ) where serviceId is related to the SERVICE template parameter
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
template<class SERVICE>
std::vector< ::boost::shared_ptr< SERVICE > > getServices( ::fwTools::Object::sptr obj ) ;

/**
 * @brief Get the service number _index of type serviceId which is attached to obj
 * @note If the service number _index is not attached to obj, it adds services of type serviceId using the add( ::fwTools::Object::sptr obj, std::string serviceId) until there are _index+1 services of this type attached to the object.
 * @note The template method is also available, where the template parameter SERVICE relates the service type serviceId
 * @todo Do not automatically add service, raise an error instead
 * @return the requested service, null if not
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API ::fwServices::IService::sptr get( ::fwTools::Object::sptr obj, std::string serviceId, unsigned int _index = 0 ) throw(fwTools::Failed );

template<class SERVICE>
::boost::shared_ptr< SERVICE > get( ::fwTools::Object::sptr obj, unsigned int _index = 0 ) throw(fwTools::Failed );

/**
 * @brief Indirectly invoke the get( ::fwTools::Object::sptr obj, std::string serviceId, unsigned int _index = 0 ), where SERVICE relates serviceId
 * @deprecated Traditional pointers should not be used
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
template<class SERVICE>
::boost::shared_ptr< SERVICE > get( fwTools::Object * obj, unsigned int _index = 0 ) throw(fwTools::Failed );

/**
 * @brief Return a registered IService having uid as unique universal identifier
 * @note indirectly invoke the ::fwTools::UUID::get method, hide the access to this API in order to provide a single unique API mixing both uuid and service/object concepts
 * @todo add the get method for object to be homogeneous
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API ::fwServices::IService::sptr get( std::string uid ) throw(fwTools::Failed );

/**
 * @brief Same as the get method, but with service type specified by a string and an uid
 *
 * Invoke the getServices( ::fwTools::Object::sptr obj , std::string serviceId ) and find out the service having the unique universal identifier uuid
 * @note a method without requiring serviceId should be sufficient and perhaps easier to use
 * @todo a get( ::fwTools::Object::sptr obj, std::string uid ) method
 * @return null if not found
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API ::fwServices::IService::sptr get( ::fwTools::Object::sptr obj, std::string serviceId, std::string uid ) throw(fwTools::Failed );

/**
 * @brief Invoke the get( ::fwTools::Object::sptr obj, std::string serviceId, std::string uid ) where serviceId is associated to the template parameter SERVICE
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
template<class SERVICE>
::boost::shared_ptr< SERVICE > get( ::fwTools::Object::sptr obj, std::string uid ) throw(fwTools::Failed );

/**
 * @brief return the _service index (or position) in the vector of services which are attached to the same object and have the same service type
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API unsigned int getServicePosition( IService::sptr  _service ) ;

/**
 * @brief return a vector containing all services associated with the object _obj
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API std::vector< ::boost::shared_ptr< IService > > getRegisteredServices( ::boost::shared_ptr< fwTools::Object > _obj );

/**
 * @brief Same as getServicePosition( IService::sptr ) but with a traditional pointer
 * @deprecated Traditional pointers should not be used
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API unsigned int getServicePosition( IService * _service ) ;

//@}


}

#include "fwServices/op/Get.hxx"

#endif /*_FWSERVICES_GET_HPP_*/
