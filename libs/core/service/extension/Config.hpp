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

#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/Object.hpp>

#include <map>

namespace sight::service
{

namespace extension
{

/**
 */
class SERVICE_CLASS_API ServiceConfigInfo : public core::BaseObject
{
public:

    SIGHT_DECLARE_CLASS(ServiceConfigInfo, core::BaseObject, new ServiceConfigInfo);

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
    core::runtime::ConfigurationElement::csptr config;
};

/**
 * @brief This class allows to register all the service configuration which has the point extension
 *        "::sight::service::extension::Config".
 *
 * Registry config like :
 * @code{.xml}
    <extension implements="::sight::service::extension::Config">
        <id>SDBOpenIOSelectorConfig</id>
        <service>module::ui::base::editor::SSelector</service>
        <desc>"Open" action's IOSelector config</desc>
        <config>
            <type mode="reader" />
            <selection mode="include" />
            <addSelection service="::sight::module::io::atoms::SReader" />
            <config id="MDAtomsConfig" service="::sight::module::io::atoms::SReader" />
        </config>
    </extension>
   @endcode
 */
class SERVICE_CLASS_API Config : public core::BaseObject
{
public:

    SIGHT_DECLARE_CLASS(Config, core::BaseObject, new Config);

    /// Return the default global instance of Config
    SERVICE_API static Config::sptr getDefault();

    /// Destructor
    SERVICE_API virtual ~Config();

    /**
     * @brief Parses module information to retrieve service declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    SERVICE_API void parseBundleInformation();

    /**
     * @brief Register a new service configuration
     * @param configId the identifier of the registered configuration.
     * @param service  the implementation of the service
     * @param desc     the description of the configuration
     * @param config   the registered config
     * @note This method is thread safe
     */
    SERVICE_API void addServiceConfigInfo(
        const std::string& configId,
        const std::string& service,
        const std::string& desc,
        core::runtime::ConfigurationElement::csptr config
    );

    /**
     * @brief Returns the configuration with the given id for the service with the given implementation
     * @note This method is thread safe
     */
    SERVICE_API core::runtime::ConfigurationElement::csptr getServiceConfig(
        const std::string& configId,
        const std::string& serviceImpl =
        ""
    ) const;

    /**
     * @brief Returns the description of the given configuration name.
     * @note This method is thread safe
     */
    SERVICE_API const std::string& getConfigDesc(const std::string& configId) const;

    /**
     * @brief Returns a vector containing the names of the available config for the service with the given
     * implementation
     * @note This method is thread safe
     */
    SERVICE_API std::vector<std::string> getAllConfigForService(
        std::string serviceImpl,
        bool matchingOnly = false
    ) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    SERVICE_API void clearRegistry();

protected:

    typedef std::map<std::string, ServiceConfigInfo::sptr> Registry;

    /// Container of service information <configId, service config information>
    Registry m_reg;

    /// Constructor
    SERVICE_API Config();

    const static std::string CONFIG_EXT_POINT;

    /// Used to protect the registry access.
    mutable core::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the service config.
    static Config::sptr s_currentServiceConfig;
};

} // namespace extension

} // namespace sight::service
