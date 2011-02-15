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
 * @class   AppConfigManager.
 * @brief   This class provides an API to manage config template.
 *
 * @author IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWSERVICES_CLASS_API AppConfigManager : public ::fwTools::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (AppConfigManager)(::fwTools::Object), (()), new AppConfigManager );

    /// Constructor. Do nothing.
    FWSERVICES_API AppConfigManager();

    /// Constructor. Do nothing.
    //FWSERVICES_API AppConfigManager( const std::string & _configName, const std::string & _configType, const std::map< std::string, std::string > & _fieldAdaptors );

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~AppConfigManager();

    /// Set config param
    //FWSERVICES_API void setConfig( const std::string & _configName, const std::string & _configType );

    /// Set config param
    FWSERVICES_API void setConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem );

    /// Set field adaptors
    //FWSERVICES_API void addField( std::string _pattern, std::string _value );

    /// Set field adaptors
    //FWSERVICES_API void setFieldAdaptors( const std::map< std::string, std::string > & _fieldAdaptors );

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
    // FWSERVICES_API static std::string getUniqueIdentifier( std::string _serviceUid = "", bool _useCpt = false );

    /// Return state
    FWSERVICES_API bool isCreated()  { return m_state == CONFIG_IS_CREATED;}
    FWSERVICES_API bool isStarted()  { return m_state == CONFIG_IS_STARTED;}
    FWSERVICES_API bool isStopped()  { return m_state == CONFIG_IS_STOPPED;}
    FWSERVICES_API bool isDestroyed(){ return m_state == CONFIG_IS_DESTROYED;}

private:

    ::fwTools::Object::sptr newObject( ::fwRuntime::ConfigurationElement::csptr _cfgElement );

    void addServicesToObjectFromCfgElem( ::fwTools::Object::sptr _obj, ::fwRuntime::ConfigurationElement::csptr _cfgElement );

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

    ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::csptr _elt );
    ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId , std::string uid="");


    typedef enum { CONFIG_IS_CREATED,
                   CONFIG_IS_STARTED,
                   CONFIG_IS_STOPPED,
                   CONFIG_IS_DESTROYED,
    } ConfigManagementState;

    ::fwTools::Object::sptr m_configRoot;

    ::fwServices::IXMLParser::sptr m_objectParser;

    ::fwRuntime::ConfigurationElement::csptr m_adaptedConfig;

    ConfigManagementState m_state;

    void start( ::fwRuntime::ConfigurationElement::csptr _elt );
    void update(::fwRuntime::ConfigurationElement::csptr _elt );
};

}

#include "fwServices/AppConfigManager.hxx"

#endif // _FWSERVICES_CONFIGTEMPLATEMANAGER_HPP_
