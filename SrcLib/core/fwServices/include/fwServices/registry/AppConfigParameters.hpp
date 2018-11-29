/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP__
#define __FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP__

#include "fwServices/config.hpp"
#include "fwServices/registry/AppConfig.hpp"

#include <fwCore/mt/types.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwTools/Object.hpp>

#include <map>

namespace fwServices
{

namespace registry
{

/**
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "::fwServices::registry::AppConfigParameters".
 */
class FWSERVICES_CLASS_API AppConfigParameters : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (AppConfigParameters)(::fwCore::BaseObject), (()), new AppConfigParameters);

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
    FWSERVICES_API const FieldAdaptorType& getParameters( const std::string& extensionId ) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    FWSERVICES_API void clearRegistry();

protected:

    typedef std::map< std::string, FieldAdaptorType > Registry;

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

#endif // __FWSERVICES_REGISTRY_APPCONFIGPARAMETERS_HPP__

