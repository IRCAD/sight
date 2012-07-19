/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_REGISTRY_APPCONFIG_HPP_
#define _FWSERVICES_REGISTRY_APPCONFIG_HPP_

#include <map>

#include <fwTools/Object.hpp>
#include <fwTools/macros.hpp>
#include <fwTools/Factory.hpp>

#include <fwCore/mt/types.hpp>

#include <fwData/Composite.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/config.hpp"


namespace fwServices
{

namespace registry
{


/**
 * @class AppInfo
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API AppInfo : public ::fwCore::BaseObject
{
public :

    enum ConfigType {
        TEMPLATE,
        STANDARD,
        PARAMETERS
    };

    fwCoreClassDefinitionsWithFactoryMacro( (AppInfo)(::fwCore::BaseObject), (()), new AppInfo );

    /// Constructor, do nothing.
    AppInfo(){}

    /// Destructor, do nothing.
    virtual ~AppInfo(){}

    ConfigType type;
    std::string group;
    std::string desc;
    typedef std::map< std::string, std::string > ParamatersType;
    ParamatersType parameters;
    ::fwRuntime::ConfigurationElement::csptr config;
};


/**
 * @class AppConfig
 * @brief This class allows to register all the configuration which has the point extension
 *        "::fwServices::registry::AppConfig".
 * @author  IRCAD (Research and Development Team).
 * @date 2012.
 */
class FWSERVICES_CLASS_API AppConfig : public ::fwCore::BaseObject
{

public:

    /// Associations of <pattern, value>.
    typedef std::map< std::string, std::string > FieldAdaptorType;

    fwCoreClassDefinitionsWithFactoryMacro( (AppConfig)(::fwCore::BaseObject), (()), new AppConfig) ;

    /// Destructor
    FWSERVICES_API virtual ~AppConfig();

    /**
     * @brief Parses bundle information to retrieve configuration declaration.
     * @note This method is thread safe
     */
    FWSERVICES_API void parseBundleInformation();

    /**
     * @brief Register a new config.
     * @param configId the identifier of the registered config.
     * @param type the type of the added config. It can be TEMPLATE, STANDARD or PARAMETERS.
     * @param group the name of the group that owns the config.
     * @param desc the config description.
     * @param parameters the list of template parametrs in the config.
     * @param config the registered config.
     * @note This method is thread safe
     */
    FWSERVICES_API void addAppInfo
    (   const std::string & configId,
            AppInfo::ConfigType type,
            const std::string & group,
            const std::string & desc,
            const AppInfo::ParamatersType & parameters,
            ::fwRuntime::ConfigurationElement::csptr config);

    /**
     * @brief  Return the config with the identifier configId.
     * @param configId : the identifier of the requested config.
     * @note This method is thread safe.
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getStandardConfig( const std::string & configId ) const;

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields associations between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getAdaptedTemplateConfig( const std::string & configId, const FieldAdaptorType & replaceFields ) const;

    /**
     * @brief  Return the adapted config with the identifier configId.
     * @param configId the identifier of the requested config.
     * @param replaceFields composite of association between the value and the pattern to replace in the config.
     * @note This method is thread safe.
     */
    FWSERVICES_API ::fwRuntime::ConfigurationElement::csptr getAdaptedTemplateConfig( const std::string & configId, ::fwData::Composite::csptr replaceFields ) const;

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
    FWSERVICES_API static std::string getUniqueIdentifier( std::string _serviceUid = "", bool _useCpt = false );

    /// Return an instance of AppConfig.
    FWSERVICES_API static AppConfig::sptr getDefault();

protected :

    typedef std::map< std::string, AppInfo::sptr > Registry;

    /// Container of <configId, appConfig information>
    Registry m_reg;

    /// Constructor
    FWSERVICES_API AppConfig();

private :

    /// Convert the composite into map <pattern, value>.
    FieldAdaptorType compositeToFieldAdaptor( ::fwData::Composite::csptr fieldAdaptors ) const;

    /// Adapts the configuration : replace field thanks to field adaptors
    ::fwRuntime::EConfigurationElement::sptr adaptConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem, const FieldAdaptorType & fieldAdaptors ) const;

    /// Adapts field thanks to field adaptors
    std::string adaptField( const std::string & _str, const FieldAdaptorType & fieldAdaptors ) const;

    /// Used to protect the registry access.
    mutable ::fwCore::mt::ReadWriteMutex m_registryMutex;

    /// Used to protect the unique identifier creation.
    static ::fwCore::mt::Mutex s_idMutex;

    /// The global instance of the app config.
    static AppConfig::sptr s_currentAppConfig;
};

} // namespace registry

} // namespace fwServices

#endif // _FWSERVICES_REGISTRY_APPCONFIG_HPP_


