/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_BUNDLE_RUNTIME_HPP_
#define _FWSERVICES_BUNDLE_RUNTIME_HPP_

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwTools/Object.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"
#include "fwServices/config.hpp"

namespace fwServices
{

namespace bundle
{

/**
 * @brief Return extensions named _identifier
 * @note Complementary with fwRuntime::Runtime::findExtension
 * @todo Move to the fwRuntime library
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > findExtensions(std::string _identifier ) ;

/**
 * @brief Return extensions contributing to extension point or point identified by the _identified parameter
 * @note Complementary with ::fwRuntime::Runtime::findExtensionsForPoint
 * @note Move to fwRuntime library
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > findExtensionsForPoint(std::string _identifier ) ;

/**
 * @brief Return service compliant extensions contributing to the point (or extension point) named _identified
 * @note A configuration compliant extension is an extension having only one configuration element (one child)
 * @note Use the findExtensionsForPoint(std::string ) method
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > findConfigurationExtensionsForPoint(std::string _identifier ) ;

/**
 * @brief Return the configuration element associated with an extension implementing _pointId with id="_cfgId"
 * @note Use findConfigurationExtensionsForPoint(std::string ) method
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::fwRuntime::ConfigurationElement::sptr findConfigurationForPoint(std::string _cfgId , std::string _pointId ) ;

/**
 * @brief Return runtime extension identifier contributing to both serviceId and object.classname()
 * @note mode=0 : search for generic (for ::fwTools::Object) and specific (for object)
 * @note mode=1 : search for specific (for object)
 * @note mode=2 : search for generic (for ::fwTools::Object)
 * @note <ext id="id" implements="serviceId" />
 * @note <ext id="id" implements="object.className()" />
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API std::vector< std::string > getValidExtensionIdsForObjectAndService( fwTools::Object::sptr object, std::string serviceId, unsigned int mode = 0 ) ;

/**
 * @brief return runtime extension whose id contributes to SERVICECLASS.classname()
 * @note <ext id="id" implements="SERVICECLASS.className()" />
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::boost::shared_ptr< ::fwRuntime::Extension > getRuntimeExtension( std::string extId , std::string serviceId ) ;


} // namespace bundle

} // namespace fwServices

#endif // _FWSERVICES_BUNDLE_RUNTIME_HPP_
