/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "service/config.hpp"
#include "service/extension/AppConfig.hpp"

#include <core/mt/types.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/EConfigurationElement.hpp>
#include <core/tools/Object.hpp>

#include <map>

namespace sight::service
{

namespace extension
{

/**
 * @brief This class allows to register all the configuration parameters which has the point extension
 *        "::sight::service::extension::AppConfigParameters".
 */
class SERVICE_CLASS_API AppConfigParameters : public core::BaseObject
{

public:

    fwCoreClassMacro(AppConfigParameters, core::BaseObject, new AppConfigParameters)

    /// Return the default global instance of AppConfigParameters
    SERVICE_API static AppConfigParameters::sptr getDefault();

    /// Destructor
    SERVICE_API virtual ~AppConfigParameters();

    /**
     * @brief Parse module information to retrieve config parameters declaration
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     **/
    SERVICE_API void parseBundleInformation();

    /**
     * @brief Get the parameters associated to extension id.
     * @note This method is thread safe.
     **/
    SERVICE_API const FieldAdaptorType& getParameters( const std::string& extensionId ) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    SERVICE_API void clearRegistry();

protected:

    typedef std::map< std::string, FieldAdaptorType > Registry;

    /// Container of parameter information
    Registry m_reg;

    /// Constructor
    SERVICE_API AppConfigParameters();

    /// Used to protect the registry access.
    mutable core::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the app config parameters.
    static AppConfigParameters::sptr s_appConfigParameters;

};

} // namespace extension

} // namespace sight::service
