/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

namespace library
{

/**
 * @brief Class factory based support method
 *
 * @note Invoked by fwServices facade methods (e.g. see support( ::fwTools::Object::sptr , std::string ))
 * @author IRCAD (Research and Development Team).
 *
 */
FWSERVICES_API bool support( ::fwTools::Object::sptr obj , std::string serviceId ) throw() ;

///**
// * @brief Class factory add method
// *
// * @note Invoked by fwServices facade methods (e.g. see add( ::fwTools::Object::sptr , std::string , std::string ) )
// * @author IRCAD (Research and Development Team).
// *
// */
//FWSERVICES_API ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId ) ;

} // namespace library

} // namespace fwServices

#endif // _FWSERVICES_LIBRARY_FACTORY_HPP_
