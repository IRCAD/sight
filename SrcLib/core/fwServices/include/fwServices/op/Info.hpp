/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_INFO_HPP_
#define _FWSERVICES_INFO_HPP_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @name Methods checking whether an object has or not a given service
 */

//@{

/**
 * @brief return true is obj has at least one service of type serviceId
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API bool has( ::fwTools::Object::sptr obj , std::string serviceId) throw();

/**
 * @brief return true is the system has an object associated with uuid
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API bool has( ::std::string uuid ) throw();

/**
 * @brief return true is obj has at least one service of type SERVICE
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
template<class SERVICE>
bool has( ::fwTools::Object::sptr obj) throw();

//@}

/**
 * @name Methods checking whether an object support or not a given service and possible available implementations
 */

//@{

/**
 * @brief Check whether an object (obj) supports service of type serviceId
 *
 * It also use component description to check whether serviceId type is supported although not registered in the class registry
 *
 * @return true if service type supported
 *
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API bool support( ::fwTools::Object::sptr obj, std::string serviceId) throw();

/**
 * @brief Check whether an object (obj) supports service of type serviceId and implementation implementationId
 *
 * It also use component description to check whether the service is supported although not registered in the class registry
 * @note Invoke the getImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId) method
 * @return true if service type supported
 *
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API bool support( ::fwTools::Object::sptr obj, std::string serviceId, std::string implementationId ) throw();

/**
 * @brief Check whether an object (obj) supports service of type SERVICE
 *
 * @return true if service type supported
 * @note invoke support( ::fwTools::Object::sptr obj, std::string serviceId
 *
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
template<class SERVICE>
bool support( ::fwTools::Object::sptr obj) throw();

/**
 * @brief Gives the identifiers of implementation of service type serviceId which could be created and attached to the given object obj according to its type
 * @return A vector of appropriate identifiers
 *
 * The returned container is organized as following: the elements concern specific implementation (i.e. the one being only compliant with the real instanciated type of obj) and then generic
 * ones (compliant with any type of object). Note that for both specific and generic implementations, the search concerns services already available in the class registry first and the one
 * (virtually) available in unstarted components
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API std::vector< std::string > getImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId) ;

/**
 * @brief Invoke the getImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId) method where the template parameter SERVICE is related to serviceId
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
template<class SERVICE>
std::vector< std::string > getImplementationIds( ::fwTools::Object::sptr obj ) ;

/**
 * @brief Returns the default implementation identifier for a given service type (serviceId) and object (obj)
 * At this time, the default implementation identifier is the first one returned by invoking the getImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId) method
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API std::string getDefaultImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId) ;

//@}



}

#include "fwServices/op/Info.hxx"

#endif /*_FWSERVICES_INFO_HPP_*/
