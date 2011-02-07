/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_CONFIGTEMPLATEMANAGER_HPP_
#define _FWSERVICES_CONFIGTEMPLATEMANAGER_HPP_

#include <map>
#include <string>

#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include "fwServices/config.hpp"

#include "fwServices/IService.hpp"
#include "fwServices/IXMLParser.hpp"

namespace fwServices
{

/**
 * @class   ConfigTemplateManager.
 * @brief   This class provides an API to manage config template.
 *
 * @author IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWSERVICES_CLASS_API ConfigTemplateManager : public fwTools::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ConfigTemplateManager)(::fwTools::Object), (()), new ConfigTemplateManager );

    /// Constructor. Do nothing.
    FWSERVICES_API ConfigTemplateManager();

    /// Constructor. Do nothing.
    FWSERVICES_API ConfigTemplateManager( const std::string & _configName, const std::string & _configType, const std::map< std::string, std::string > & _fieldAdaptors );

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~ConfigTemplateManager();

    /// Set config param
    FWSERVICES_API void setConfig( const std::string & _configName, const std::string & _configType );

    /// Set config param
    FWSERVICES_API void setConfig( ::fwRuntime::ConfigurationElement::sptr _cfgElem );

    /// Set field adaptors
    FWSERVICES_API void addField( std::string _pattern, std::string _value );

    /// Set field adaptors
    FWSERVICES_API void setFieldAdaptors( const std::map< std::string, std::string > & _fieldAdaptors );

    /// Get config root
    FWSERVICES_API ::fwTools::Object::sptr getConfigRoot() const;

    /// Get config root with autocast
    template < class CLASSNAME >
    SPTR(CLASSNAME) getConfigRoot() const;

    /// Creates objects and services from config
    FWSERVICES_API void create();

    /// Starts services specified in config
    FWSERVICES_API void start();

    /// Updates services specified in config
    FWSERVICES_API void update();

    /// Calls methods : create, start then update.
    FWSERVICES_API void launch();

    /// Stops services specified in config
    FWSERVICES_API void stop();

    /// Destroys services specified in config
    FWSERVICES_API void destroy();

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    FWSERVICES_API void stopAndDestroy();

    /// Create an unique identifier
    FWSERVICES_API static std::string getUniqueIdentifier( std::string _serviceUid = "", bool _useCpt = false );

    /// Return state
    FWSERVICES_API bool isCreated()  { return m_state == CONFIG_IS_CREATED;}
    FWSERVICES_API bool isStarted()  { return m_state == CONFIG_IS_STARTED;}
    FWSERVICES_API bool isStopped()  { return m_state == CONFIG_IS_STOPPED;}
    FWSERVICES_API bool isDestroyed(){ return m_state == CONFIG_IS_DESTROYED;}

private:

    ::fwTools::Object::sptr newObject( ::fwRuntime::ConfigurationElement::sptr _cfgElement );
    void addServicesToObjectFromCfgElem( ::fwTools::Object::sptr _obj, ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    typedef std::vector< ::fwServices::IService::wptr > ServiceContainer;
    ServiceContainer m_createdServices;
    ServiceContainer m_startedServices;
    ServiceContainer m_startedComChannelServices;

    void startComChannel();
    void stopComChannel();


    ::fwTools::Object::sptr createNewObject( bool _hasAttributeType,
                                             const std::string & _type,
                                             bool _hasAttributeUid,
                                             const std::string & _uid,
                                             bool _hasAttributeId,
                                             const std::string & _id );

    ::fwTools::Object::sptr getAlreadyDefinedObject(
            const std::string & _uid,
            bool _hasAttributeType,
            const std::string & _type,
            bool _hasAttributeId,
            const std::string & _id );

    ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt );
    ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId , std::string uid="");


    typedef enum { CONFIG_IS_CREATED,
                   CONFIG_IS_STARTED,
                   CONFIG_IS_STOPPED,
                   CONFIG_IS_DESTROYED,
    } ConfigManagementState;

    /// Adapts the configuration : replace field thanks to field adaptors
    ::fwRuntime::EConfigurationElement::sptr adaptConfig( ::fwRuntime::ConfigurationElement::sptr _cfgElem );

    /// Adapts field thanks to field adaptors
    std::string adaptField( const std::string & _str ) const;

    /// Get the object service configuration given by the service config
    void loadConfig();

    std::string m_configName;

    std::string m_configType;

    std::map< std::string, std::string > m_fieldAdaptors;

    ::fwTools::Object::sptr m_configRoot;

    ::fwServices::IXMLParser::sptr m_objectParser;

    ::fwRuntime::ConfigurationElement::sptr m_configTemplate;

    ::fwRuntime::ConfigurationElement::sptr m_adaptedConfig;

    ConfigManagementState m_state;


    void start( ::fwRuntime::ConfigurationElement::sptr _elt );
};

}

#include "fwServices/ConfigTemplateManager.hxx"

#endif // _FWSERVICES_CONFIGTEMPLATEMANAGER_HPP_
