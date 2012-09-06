/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP_
#define _FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP_

#include <map>

#include <fwCore/mt/types.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/config.hpp"

#include "fwServices/registry/AppConfig.hpp"

namespace fwServices
{

namespace registry
{

/**
 * @class AppConfigParameters
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "::fwServices::registry::AppConfigParameters".
 * @author  IRCAD (Research and Development Team).
 * @date 2012
 */
class FWSERVICES_CLASS_API AppConfigParameters : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (AppConfigParameters)(::fwCore::BaseObject), (()), new AppConfigParameters) ;

    /// Return the default global instance of AppConfigParameters
    FWSERVICES_API static AppConfigParameters::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~AppConfigParameters();

    /**
     * @brief Parse bundle information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     **/
    FWSERVICES_API void parseBundleInformation();

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    FWSERVICES_API const AppConfig::FieldAdaptorType & getParameters( const std::string & extensionId ) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    FWSERVICES_API void clearRegistry();

protected :

    typedef std::map< std::string, AppConfig::FieldAdaptorType > Registry;

    /// Container of parameter information
    Registry m_reg;

    /// Constructor
    FWSERVICES_API AppConfigParameters();

    /// Used to protect the registry access.
    mutable ::fwCore::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the app config parameters.
    static AppConfigParameters::sptr s_appConfigParameters;


};

} // namespace registry

} // namespace fwServices

#endif // _FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP_


