/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/io/Validator.hpp"
#include "fwRuntime/operations.hpp"
#include "fwRuntime/Runtime.hpp"

namespace fwRuntime
{
/**
 * @brief   Check configuration element (its XML representation) with respect to the validator (referencing the xsd
 * schema)
 * @note    Used to check any service/object based configurations as well as runtime extensions.
 * @return  (true,"") if configuration element is correct vs validator, else (false,std::string(error message))
 * otherwise
 *

 */
FWRUNTIME_API std::pair< bool, std::string > validateConfigurationElement(
    std::shared_ptr< ::fwRuntime::io::Validator >   _validator, ::fwRuntime::ConfigurationElement::sptr _element );

/**
 * @brief   Update pNode content according to _cfgElement
 * @note    Used to validate a configuration element vs a xsd schema.
 * @note    Values are not managed yet (cases where curChild->type == XML_TEXT_NODE ).
 * @note    To be integrated to configuration element.
 *

 */
FWRUNTIME_API void ConfigurationElement2XML(::fwRuntime::ConfigurationElement::sptr _cfgElement, xmlNodePtr pNode);

/**
 * @brief       Returns the configuration element embedding the configuration to be considered for initializing an
 * object or a service
 * @param[in]   _config : configuration element containing an attribute "config" specifying the name of the contribution
 * to consider
 * @param[in]   _extension_pt : extension point to which the configuration should contribute with the name indicated by
 * the previous "config" attribute value
 *
 */
FWRUNTIME_API ConfigurationElement::sptr getCfgAsAnExtension( ConfigurationElement::sptr _config,
                                                              std::string _extension_pt );

/**
 * @brief   Returns configuration elements extending the _extension_pt extension point
 *

 */
FWRUNTIME_API std::vector< ConfigurationElement::sptr > getAllCfgForPoint( std::string _extension_pt );

/**
 * @brief   Returns extensions extending the _extension_pt extension point
 *

 */
FWRUNTIME_API std::vector< std::shared_ptr< ::fwRuntime::Extension > > getAllExtensionsForPoint(
    std::string extension_pt);

/**
 * @brief       Returns contribution ids and configuration elements related to a given extension point
 * @param[in]   _extension_pt : identifier of the extension point to which configuration elements contribute
 * @return      Map associating contribution ids and associated configuration elements contributing to the extension
 * point _extension_pt
 * @note        Configuration element identifiers are value of attributes "id".
 *
 */
FWRUNTIME_API std::map< std::string, ConfigurationElement::sptr > getAllIdAndConfigurationElementsForPoint(
    std::string _extension_pt  );
/**
 * @brief       Returns contribution ids to a given extension point
 * @param[in]   _extension_pt : identifier of the extension point to which configuration elements contribute
 * @return      Map associating contribution ids and associated configuration elements contributing to the extension
 * point _extension_pt
 * @note        Configuration element identifiers are value of attributes "id".
 *
 */
FWRUNTIME_API std::vector< std::string > getAllIdsForPoint( std::string _extension_pt  );

/**
 * @brief   Get information for the point
 * @return  a string with the information of the extension point
 *
 */
FWRUNTIME_API std::string getInfoForPoint( std::string _extension_pt);

}
