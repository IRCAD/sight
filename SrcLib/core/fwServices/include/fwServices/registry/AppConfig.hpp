/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_APPCONFIG_HPP__
#define __FWSERVICES_REGISTRY_APPCONFIG_HPP__

#include "fwServices/config.hpp"
#include "fwServices/registry/AppInfo.hpp"

#include <fwCore/mt/types.hpp>

#include <fwData/Composite.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwTools/macros.hpp>
#include <fwTools/Object.hpp>

#include <map>


namespace fwServices
{

namespace registry
{

/**
 * @brief This class allows to register all the configuration which has the point extension
 *        "::fwServices::registry::AppConfig".
 */
class FWSERVICES_CLASS_API AppConfig : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (AppConfig)(::fwCore::BaseObject), (()), new AppConfig);

    /// Destructor
    FWSERVICES_API virtual ~AppConfig();

    /**
     * @brief Parses bundle information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    FWSERVICES_API void parseBundleInformation();

    /**
     * @brief Register a new config.
     * @param configId the identifier of the registered config.
     * @param type the type of the added config. It can be TEMPLATE, STANDARD or PARAMETERS.
     * @param group the name of the group that owns the config.
     * @param desc the config description.
     * @param parameters the list of template parameters in the config.
     * @param config the registered config.
     * @note This method is thread safe
     */
    FWSERVICES_API void addAppInfo (   const std::string & configId,
                                       const std::string & group,
                                       const std::string & desc,
                                       const AppInfo::ParametersType & parameters,
                                       ::fwRuntime::ConfigurationElement::csptr config,
                                       const std::string bundleId,
                                       const std::string bundleVersion);

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields associations between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getAdaptedTemplateConfig( const std::string & configId,
                                                                                      const FieldAdaptorType replaceFields =
                                                                                          FieldAdaptorType() ) const;

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields composite of association between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getAdaptedTemplateConfig( const std::string & configId,
                                                                                      ::fwData::Composite::csptr replaceFields )
    const;

    /**
     * @brief Retrieves the bunble from the config id
     * @param configId the config identifier
     */
    FWSERVICES_API std::shared_ptr< ::fwRuntime::Bundle > getBundle(const std::string & configId);

    /**
     * @brief Return all configurations ( standard and template ) register in the registry.
     * @note This method is thread safe.
     */
    FWSERVICES_API std::vector< std::string > getAllConfigs() const;

    /**
     * @brief Return all configurations with specified group.
     * @note This method is thread safe.
     */
    FWSERVICES_API std::vector< std::string > getConfigsFromGroup(const std::string & group) const;

    /**
     * @brief Clear the registry.
     * @note This method is thread safe.
     */
    FWSERVICES_API void clearRegistry();

    /**
     * @brief Create an unique identifier
     * @note This method is thread safe.
     */
    FWSERVICES_API static std::string getUniqueIdentifier(const std::string& serviceUid = "" );

    /// Return an instance of AppConfig.
    FWSERVICES_API static AppConfig::sptr getDefault();

protected:

    typedef std::map< std::string, AppInfo::sptr > Registry;

    /// Container of <configId, appConfig information>
    Registry m_reg;

    /// Constructor
    FWSERVICES_API AppConfig();

private:

    /// Convert the composite into map <pattern, value>.
    FieldAdaptorType compositeToFieldAdaptor( ::fwData::Composite::csptr fieldAdaptors ) const;

    /// Adapts the configuration : replace field thanks to field adaptors
    ::fwRuntime::EConfigurationElement::sptr adaptConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem,
                                                          const FieldAdaptorType & fieldAdaptors ) const;

    /// Adapts field thanks to field adaptors
    std::string adaptField( const std::string & _str, const FieldAdaptorType & fieldAdaptors ) const;

    /// Used to protect the registry access.
    mutable ::fwCore::mt::ReadWriteMutex m_registryMutex;

    /// Used to protect the unique identifier creation.
    static ::fwCore::mt::Mutex s_idMutex;

    /// The global instance of the app config.
    static AppConfig::sptr s_currentAppConfig;

    /// The static identifier for mandatory parameters.
    static std::string s_mandatoryParameterIdentifier;
};

} // namespace registry

} // namespace fwServices

#endif // __FWSERVICES_REGISTRY_APPCONFIG_HPP__


