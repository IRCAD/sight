/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwServices/config.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/Object.hpp>

#include <map>

namespace fwServices
{

namespace registry
{

/**
 */
class FWSERVICES_CLASS_API ServiceConfigInfo : public ::fwCore::BaseObject
{
public:

    fwCoreClassMacro(ServiceConfigInfo, ::fwCore::BaseObject, new ServiceConfigInfo);

    /// Constructor, do nothing.
    ServiceConfigInfo()
    {
    }

    /// Destructor, do nothing.
    virtual ~ServiceConfigInfo()
    {
    }

    std::string service;
    std::string desc;
    ::fwRuntime::ConfigurationElement::csptr config;
};

/**
 * @brief This class allows to register all the service configuration which has the point extension
 *        "::fwServices::registry::ServiceConfig".
 *
 * Registry config like :
 * @code{.xml}
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>SDBOpenIOSelectorConfig</id>
        <service>::uiIO::editor::SIOSelector</service>
        <desc>"Open" action's IOSelector config</desc>
        <config>
            <type mode="reader" />
            <selection mode="include" />
            <addSelection service="::ioAtoms::SReader" />
            <config id="MDAtomsConfig" service="::ioAtoms::SReader" />
        </config>
    </extension>
   @endcode
 */
class FWSERVICES_CLASS_API ServiceConfig : public ::fwCore::BaseObject
{

public:

    fwCoreClassMacro(ServiceConfig, ::fwCore::BaseObject, new ServiceConfig);

    /// Return the default global instance of ServiceConfig
    FWSERVICES_API static ServiceConfig::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~ServiceConfig();

    /**
     * @brief Parses bundle information to retrieve service declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    FWSERVICES_API void parseBundleInformation();

    /**
     * @brief Register a new service configuration
     * @param configId the identifier of the registered configuration.
     * @param service  the implementation of the service
     * @param desc     the description of the configuration
     * @param config   the registered config
     * @note This method is thread safe
     */
    FWSERVICES_API void addServiceConfigInfo( const std::string& configId,
                                              const std::string& service,
                                              const std::string& desc,
                                              ::fwRuntime::ConfigurationElement::csptr config );

    /**
     * @brief Returns the configuration with the given id for the service with the given implementation
     * @note This method is thread safe
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getServiceConfig( const std::string& configId,
                                                                              const std::string& serviceImpl =
                                                                                  "" ) const;

    /**
     * @brief Returns the description of the given configuration name.
     * @note This method is thread safe
     */
    FWSERVICES_API const std::string& getConfigDesc( const std::string& configId ) const;

    /**
     * @brief Returns a vector containing the names of the available config for the service with the given
     * implementation
     * @note This method is thread safe
     */
    FWSERVICES_API std::vector< std::string > getAllConfigForService( std::string serviceImpl,
                                                                      bool matchingOnly = false ) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    FWSERVICES_API void clearRegistry();

protected:

    typedef std::map< std::string, ServiceConfigInfo::sptr > Registry;

    /// Container of service information <configId, service config information>
    Registry m_reg;

    /// Constructor
    FWSERVICES_API ServiceConfig();

    const static std::string CONFIG_EXT_POINT;

    /// Used to protect the registry access.
    mutable ::fwCore::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the service config.
    static ServiceConfig::sptr s_currentServiceConfig;
};

} // namespace registry

} // namespace fwServices
