/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_APPCONFIG2_HPP__
#define __FWSERVICES_REGISTRY_APPCONFIG2_HPP__

#include "fwServices/config.hpp"
#include "fwServices/registry/AppInfo.hpp"

#include <fwCore/mt/types.hpp>

#include <fwData/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
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
 *        "::fwServices::registry::AppConfig2".
 */
class FWSERVICES_CLASS_API AppConfig2 : public ::fwCore::BaseObject
{

public:

    /// Associations of <pattern, value>.
    typedef std::map< std::string, std::string > FieldAdaptorType;

    fwCoreClassDefinitionsWithFactoryMacro( (AppConfig2)(::fwCore::BaseObject), (()), new AppConfig2);

    /// Destructor
    FWSERVICES_API virtual ~AppConfig2();

    /**
     * @brief Parses bundle information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    FWSERVICES_API void parseBundleInformation();

    /**
     * @brief Parses bundle information to retrieve configuration declaration.
     * @warning This method must be launch only once. The same extension will not be parsed twice.
     * @note This method is thread safe.
     */
    FWSERVICES_API void parseBundleInformation(
        const std::vector< std::shared_ptr< ::fwRuntime::Extension > > &extensions);

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
    FWSERVICES_API void addAppInfo( const std::string & configId,
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
                                                                                      const FieldAdaptorType replaceFields,
                                                                                      bool autoPrefixId ) const;

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields composite of association between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getAdaptedTemplateConfig( const std::string & configId,
                                                                                      ::fwData::Composite::csptr replaceFields,
                                                                                      bool autoPrefixId )
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

    /// Return an instance of AppConfig2.
    FWSERVICES_API static AppConfig2::sptr getDefault();

protected:

    typedef std::map< std::string, AppInfo::sptr > Registry;

    /// Container of <configId, appConfig2 information>
    Registry m_reg;

    /// Constructor
    FWSERVICES_API AppConfig2();

private:

    /// Convert the composite into map <pattern, value>.
    FieldAdaptorType compositeToFieldAdaptor( ::fwData::Composite::csptr fieldAdaptors ) const;

    /// Adapts the configuration : replace field thanks to field adaptors
    ::fwRuntime::EConfigurationElement::sptr adaptConfig(::fwRuntime::ConfigurationElement::csptr _cfgElem,
                                                         const FieldAdaptorType & fieldAdaptors,
                                                         const std::string &autoPrefixId ) const;

    /// Adapts field thanks to field adaptors
    std::string adaptField( const std::string & _str, const FieldAdaptorType & fieldAdaptors ) const;

    /// Used to protect the registry access.
    mutable ::fwCore::mt::ReadWriteMutex m_registryMutex;

    /// Used to protect the unique identifier creation.
    static ::fwCore::mt::Mutex s_idMutex;

    /// The global instance of the app config.
    static AppConfig2::sptr s_currentAppConfig2;

    /// The static identifier for mandatory parameters.
    static std::string s_mandatoryParameterIdentifier;
};

} // namespace registry

} // namespace fwServices

#endif // __FWSERVICES_REGISTRY_APPCONFIG2_HPP__


