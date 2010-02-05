/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWRUNTIME_HELPER_HPP_
#define FWRUNTIME_HELPER_HPP_
#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/operations.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/io/Validator.hpp"


namespace fwRuntime
{
/**
 * @brief	Check configuration element (its XML representation) with respect to the validator (referencing the xsd schema)
 * @note 	Used to check any service/object based configurations as well as runtime extensions.
 * @return 	(true,"") if configuration element is correct vs validator, else (false,std::string(error message)) otherwise
 * @author 	IRCAD (Research and Development Team).

 */
FWRUNTIME_API std::pair< bool , std::string > validateConfigurationElement( ::boost::shared_ptr< ::fwRuntime::io::Validator >	_validator , ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _element ) ;

/**
 * @brief	Update pNode content according to _cfgElement
 * @note 	Used to validate a configuration element vs a xsd schema.
 * @note 	Values are not managed yet (cases where curChild->type == XML_TEXT_NODE ).
 * @note 	To be integrated to configuration element.
 * @author 	IRCAD (Research and Development Team).

 */
FWRUNTIME_API void ConfigurationElement2XML(::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement, xmlNodePtr pNode) ;

/**
 * @brief		Returns the configuration element embedding the configuration to be considered for initializing an object or a service
 * @param[in] 	_config : configuration element containing an attribute "config" specifying the name of the contribution to consider
 * @param[in]	_extension_pt : extension point to which the configuration should contribute with the name indicated by the previous "config" attribute value
 * @author 	IRCAD (Research and Development Team).
 */
FWRUNTIME_API ::boost::shared_ptr< ConfigurationElement > getCfgAsAnExtension( ::boost::shared_ptr< ConfigurationElement > _config , std::string _extension_pt ) ;

/**
 * @brief	Returns configuration elements extending the _extension_pt extension point
 * @author 	IRCAD (Research and Development Team).

 */
FWRUNTIME_API std::vector< ::boost::shared_ptr< ConfigurationElement > > getAllCfgForPoint( std::string _extension_pt ) ;

/**
 * @brief		Returns contribution ids and configuration elements related to a given extension point
 * @param[in] 	_extension_pt : identifier of the extension point to which configuration elements contribute
 * @return 		Map associating contribution ids and associated configuration elements contributing to the extension point _extension_pt
 * @note 		Configuration element identifiers are value of attributes "id".
 * @author 	IRCAD (Research and Development Team).
 */
FWRUNTIME_API std::map< std::string , ::boost::shared_ptr< ConfigurationElement > > getAllIdAndConfigurationElementsForPoint( std::string _extension_pt  ) ;
/**
 * @brief		Returns contribution ids to a given extension point
 * @param[in]	_extension_pt : identifier of the extension point to which configuration elements contribute
 * @return 		Map associating contribution ids and associated configuration elements contributing to the extension point _extension_pt
 * @note 		Configuration element identifiers are value of attributes "id".
 * @author 	IRCAD (Research and Development Team).
 */
FWRUNTIME_API std::vector< std::string > getAllIdsForPoint( std::string _extension_pt  ) ;

/**
 * @brief 	Get information for the point
 * @return 	a string with the information of the extension point
 * @author 	IRCAD (Research and Development Team).
 */
FWRUNTIME_API std::string getInfoForPoint( std::string _extension_pt) ;

}

#endif /*HELPER_HPP_*/
