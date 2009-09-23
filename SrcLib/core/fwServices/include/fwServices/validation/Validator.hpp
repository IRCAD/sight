/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_VALIDATION_VALIDATOR_HPP_
#define _FWSERVICES_VALIDATION_VALIDATOR_HPP_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @brief Concerns validation
 *
 * Validation is based on :
 * \li Checking that involved objects and services type are reported either the Class Registry or in the component distribution
 * \li Checking that service to object association are compliant
 * \li Checking that both object and service configurations are valid (concern class attributes) with respect to XSD schema (warning when no XSD is provided)
 *
 * @note Validation can be completely performed on the base of the component distribution, without preloading or linking with dynamic libraries implementing classes.
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
namespace validation
{

/**
 * @brief Method checking validity of a configuration element, assuming that it describes an object.
 *
 * Validation process manages both in-line and off-line declarations
 * The result of the validation process is logged.
 * @note This procedure applied to all end nodes of the component based XML graph in order to check all the system
 * @note It invokes the checkService( std::string , ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > , ::std::stringstream ) method.
 */
FWSERVICES_API bool checkObject( ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _elt , std::string objType="" ) ;

/**
 * @brief Method checking the validity of a service configuration element associated to a class named implementationId
 *
 * @param [out] stream embeds the result of the validation process
 */
FWSERVICES_API bool checkService( std::string implementationId , ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _elt , ::std::stringstream &stream) ;


}
}


#endif /*HELPER_HPP_*/
