/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_BUNDLE_FACTORY_HPP_
#define _FWSERVICES_BUNDLE_FACTORY_HPP_

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
 * @brief Concerns component based object service basic manipulations
 *
 * \li Mainly consists in the support and add methods (support( ::boost::shared_ptr< fwTools::Object > , std::string ) and add( ::boost::shared_ptr< fwTools::Object > , std::string , std::string )).
 * \li Helpers have been developped for internal use (should not be used from outside) to analyze the XML based graph provided by component descriptions
 * @note It focuses on find the appropriate component to start (and dynamic libraries to load) before invoking class registry
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
namespace bundle
{

/**
 * @brief Component based support method
 *
 * @note Invoked by fwServices facade methods (e.g. see support( ::fwTools::Object::sptr , std::string ))
 * @note Mainly invoke the getValidExtensionIdsForObjectAndService( ::boost::shared_ptr< fwTools::Object > , std::string , unsigned int ) method
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API bool support( ::boost::shared_ptr< fwTools::Object > obj , std::string serviceId ) throw() ;

/**
 * @brief Component based add method
 *
 * @note Invoked by fwServices facade methods (e.g. see add( ::fwTools::Object::sptr , std::string , std::string ) )
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
FWSERVICES_API ::boost::shared_ptr< fwServices::IService > add( ::boost::shared_ptr< fwTools::Object > obj , std::string serviceId  , std::string _implementationId ) ;

} // namespace bundle

} // namespace fwServices

#endif // _FWSERVICES_BUNDLE_FACTORY_HPP_
