/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ADD_HPP_
#define _FWSERVICES_ADD_HPP_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @name Methods for creating and attaching services to object
 */
//@{

/**
 * @brief Create and attach a service of type serviceId to the object obj
 * @return the service
 * @note If several service type implementation are available, the first one is automatically chosen, being the first element return by getImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId)
 * @note the template method is also available, where the template parameter SERVICE corresponds to the serviceId
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId ) ;

template<class SERVICE>
SPTR(SERVICE) add( ::fwTools::Object::sptr obj ) ;

/**
 * @brief Create and attach a service of type serviceId and implementation _implementationId to the object obj
 * @return the service
 * @note the template method is also available, where the template parameter SERVICE corresponds to the serviceId
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId ) ;

template<class SERVICE>
SPTR(SERVICE) add( ::fwTools::Object::sptr obj , std::string _implementationId ) ;

/**
 * @brief Create and attach to the object obj a service of type serviceId, implementation _implementationId with the universal unique identifier _id
 * @return the service
 * @note the template method is also available, where the template parameter SERVICE corresponds to the serviceId
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId , std::string _id) ;

template<class SERVICE>
SPTR(SERVICE) add( ::fwTools::Object::sptr obj , std::string _implementationId , std::string _id ) ;

/**
 * @brief Create and attach to the object obj a service described in the XML like structure _elt
 * @note configuration element specifies service type and implementation (inline or offline)
 * @return the service
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt ) ;


//@}


}

#include "fwServices/op/Add.hxx"

#endif /*_FWSERVICES_ADD_HPP_*/
