/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_LIBRARY_FACTORY_HPP_
#define _FWSERVICES_LIBRARY_FACTORY_HPP_

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwTools/Object.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"
#include "fwServices/config.hpp"

namespace fwServices
{

/**
 * @brief Concerns class factory based object service basic manipulations
 *
 * Mainly consists in the support and add methods
 * @note The term library means that the program has loaded a dynamic library involving the registration of the class types in the class registry
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
namespace library
{

/**
 * @brief Class factory based support method
 *
 * @note Invoked by fwServices facade methods (e.g. see support( ::fwTools::Object::sptr , std::string ))
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API bool support( ::boost::shared_ptr< fwTools::Object > obj , std::string serviceId ) throw() ;

/**
 * @brief Class factory add method
 *
 * @note Invoked by fwServices facade methods (e.g. see add( ::fwTools::Object::sptr , std::string , std::string ) )
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API ::boost::shared_ptr< fwServices::IService > add( ::boost::shared_ptr< fwTools::Object > obj , std::string serviceId , std::string _implementationId ) ;

} // namespace library

} // namespace fwServices

#endif // _FWSERVICES_LIBRARY_FACTORY_HPP_
