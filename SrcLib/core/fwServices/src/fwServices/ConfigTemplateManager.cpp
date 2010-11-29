/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <fwRuntime/operations.hpp>

#include "fwServices/ConfigTemplateManager.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/Base.hpp"
#include "fwServices/op/Get.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/validation/Validator.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

ConfigTemplateManager::ConfigTemplateManager() :
                                            m_configName(""),
                                            m_configType(""),
                                            m_state ( CONFIG_IS_DESTROYED )
{}

//-----------------------------------------------------------------------------

ConfigTemplateManager::ConfigTemplateManager( const std::string & _configName, const std::string & _configType, const std::map< std::string, std::string > & _fieldAdaptors )
{
    setConfig( _configName, _configType );
    setFieldAdaptors( _fieldAdaptors );
}

//-----------------------------------------------------------------------------

ConfigTemplateManager::~ConfigTemplateManager()
{
    SLM_ASSERT("Sorry, you must stop the manager first before destroying it.", m_state == CONFIG_IS_DESTROYED );
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::setConfig( const std::string & _configName, const std::string & _configType )
{
    m_configName = _configName;
    m_configType = _configType;

    // Load config
    loadConfig();
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::setConfig( ::fwRuntime::ConfigurationElement::sptr _cfgElem )
{
    m_configTemplate = _cfgElem;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::setFieldAdaptors( const std::map< std::string, std::string > & _fieldAdaptors )
{
    m_fieldAdaptors = _fieldAdaptors;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::addField( std::string _pattern, std::string _value )
{
    m_fieldAdaptors[_pattern] = _value;
}


//-----------------------------------------------------------------------------

::fwTools::Object::sptr ConfigTemplateManager::getConfigRoot() const{

    return m_configRoot;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::create()
{
    SLM_ASSERT("Sorry, manager is already running and you try creating config.", m_state == CONFIG_IS_DESTROYED );

    // Adapt config
    m_adaptedConfig = adaptConfig( m_configTemplate );

    SLM_ASSERT("Sorry, the xml that describes extension "<< m_configName <<" is not valid.", ::fwServices::validation::checkObject( m_adaptedConfig ) );

    // Create object and services

#ifdef USE_SRVFAC
    m_configRoot = this->newObject( m_adaptedConfig );
#else
    m_configRoot = ::fwServices::New( m_adaptedConfig );
#endif

    m_state = CONFIG_IS_CREATED;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::start()
{
    SLM_ASSERT("Sorry, manager is not created and you try starting it.", m_state == CONFIG_IS_CREATED || m_state == CONFIG_IS_STOPPED );

    this->start( m_adaptedConfig ) ;

    m_state = CONFIG_IS_STARTED;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::update()
{
    ::fwServices::update( m_adaptedConfig ) ;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::launch()
{
    create();
    start();
    update();
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::stop()
{
    SLM_ASSERT("Sorry, manager is not started and you try stopping it.", m_state == CONFIG_IS_STARTED );

#ifdef USE_SRVFAC
    BOOST_REVERSE_FOREACH( ::fwServices::IService::wptr wsrv, m_startedServices )
    {
        SLM_ASSERT( "Sorry, CTM must stop a service, but it is expired", ! wsrv.expired());
        ::fwServices::IService::sptr srv = wsrv.lock();
        OSLM_ASSERT( "Sorry, CTM must stop a service ( uid = "<< srv->getID() <<" , classname = "<< srv->getClassname() <<" ), but it is already stopped", ! srv->isStopped() );
        OSLM_INFO("Stop service ( " << srv->getID() << " ) managed by the ConfigTemplateManager.");
        srv->stop();
    }
    m_startedServices.clear();
#else
    ::fwServices::stop( m_adaptedConfig ) ;
#endif

    m_state = CONFIG_IS_STOPPED;
    SLM_INFO( ::fwServices::OSR::getRegistryInformation() );
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::destroy()
{
    SLM_ASSERT("Sorry, manager is not stopped and you try detroying it.", m_state == CONFIG_IS_STOPPED || m_state == CONFIG_IS_CREATED );

#ifdef USE_SRVFAC

    BOOST_REVERSE_FOREACH( ::fwServices::IService::wptr wsrv, m_createdServices )
    {
        SLM_ASSERT( "Sorry, CTM must destroy a service, but it is already expired.", ! wsrv.expired());
        ::fwServices::IService::sptr srv = wsrv.lock();
        OSLM_ASSERT( "Sorry, CTM must destroy a service ( uid = "<< srv->getID() <<" , classname = "<< srv->getClassname() <<" ), but it must be stopped before.", srv->isStopped() );
        OSLM_INFO("Unregister service ( " << srv->getID() << " ) managed by the ConfigTemplateManager.");
        OSR::unregisterService( srv );
    }
    m_createdServices.clear();

#else
    ::fwServices::stopAndUnregister( m_adaptedConfig ) ;
#endif

    SLM_INFO( ::fwServices::OSR::getRegistryInformation() );

    m_adaptedConfig.reset();
    m_configTemplate.reset();
    m_configRoot.reset();

    m_state = CONFIG_IS_DESTROYED;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::stopAndDestroy()
{
    stop();
    destroy();
}

//-----------------------------------------------------------------------------

std::string ConfigTemplateManager::getUniqueIdentifier( std::string _serviceUid, bool _useCpt )
{
    std::string id;
    static unsigned int srvCpt = 1;
    std::stringstream sstr;

    if ( _serviceUid.empty() )
    {
        sstr <<  "ConfigTemplateManager_" << srvCpt;
        srvCpt++;
    }
    else
    {
        sstr <<  _serviceUid;
        if ( _useCpt )
        {
            sstr << "_" << srvCpt;
            srvCpt++;
        }
    }
    id = sstr.str();

    return id;
}

//-----------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr ConfigTemplateManager::adaptConfig( ::fwRuntime::ConfigurationElement::sptr _cfgElem )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::EConfigurationElement::NewSptr result ( _cfgElem->getName() );
    result->setValue( this->adaptField( _cfgElem->getValue() ) );

    typedef std::map<std::string, std::string> MapAttributesType;
    BOOST_FOREACH( MapAttributesType::value_type attribute, _cfgElem->getAttributes() )
    {
        result->setAttributeValue( attribute.first, this->adaptField( attribute.second ) );
    }

    BOOST_FOREACH ( ::fwRuntime::ConfigurationElement::sptr subElem, _cfgElem->getElements())
    {
        result->addConfigurationElement( this->adaptConfig( subElem ) );
    }

    return result;
}

//-----------------------------------------------------------------------------

std::string ConfigTemplateManager::adaptField( const std::string & _str ) const
{
    std::string newStr = _str;

    for (   std::map< std::string, std::string >::const_iterator fieldAdaptor = m_fieldAdaptors.begin();
            fieldAdaptor != m_fieldAdaptors.end();
            ++fieldAdaptor )
    {
        std::stringstream sstr;
        sstr << "(.*)" << fieldAdaptor->first << "(.*)";
        ::boost::regex machine_regex ( sstr.str() );
        if ( ::boost::regex_match( _str, machine_regex ) )
        {
            std::stringstream machine_format;
            machine_format << "\\1" << fieldAdaptor->second << "\\2";
            newStr = ::boost::regex_replace( newStr, machine_regex, machine_format.str(), ::boost::match_default | ::boost::format_sed );
        }
    }

    return newStr;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::loadConfig()
{
    m_configTemplate = ::fwServices::bundle::findConfigurationForPoint( m_configName, m_configType );
}

//-----------------------------------------------------------------------------

//void ConfigTemplateManager::read( ::fwRuntime::ConfigurationElement::sptr _cfgElem )
//{
//    SLM_TRACE_FUNC();
//
//    ::fwRuntime::ConfigurationElement::sptr result;
//
//    OSLM_INFO( " Name : " << _cfgElem->getName() );
//    OSLM_INFO( " Value : " << _cfgElem->getValue() );
//
//    typedef std::map<std::string, std::string> MapAttributesType;
//    BOOST_FOREACH( MapAttributesType::value_type attribute, _cfgElem->getAttributes() )
//    {
//        OSLM_INFO( " Attribute Name : " << attribute.first  << "  -  Attribute Value : " << attribute.second );
//    }
//
//    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr subElem, _cfgElem->getElements())
//    {
//        this->read( subElem );
//    }
//}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr ConfigTemplateManager::newObject( ::fwRuntime::ConfigurationElement::sptr _cfgElement )
{
    OSLM_ASSERT("Sorry method newObject( cfg ) works only on an object xml config, here xml element is : " << _cfgElement->getName(), _cfgElement->getName() == "object" );
    OSLM_ASSERT("Sorry, attribute \"id\" ( val = "<< ( _cfgElement->hasAttribute( "id" ) ? _cfgElement->getExistingAttributeValue("id") : "" ) <<" ) of an object xml elements his deprecated.", ! _cfgElement->hasAttribute( "id" ) );

    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT = "new";
    const std::string GET_OBJECT = "ref";

    // Get basic attributes

    // uid
    bool hasAttributeUid = false;
    std::string uid = "";
    if ( _cfgElement->hasAttribute( "uid" ) )
    {
        uid = _cfgElement->getExistingAttributeValue( "uid" );
        SLM_ASSERT( "Sorry uid attribute is an empty string", ! uid.empty() );
        hasAttributeUid = true;
    }

    // type
    bool hasAttributeType = false;
    std::string type = "";
    if ( _cfgElement->hasAttribute( "type" ) )
    {
        type = _cfgElement->getExistingAttributeValue( "type" );
        OSLM_ASSERT( "Sorry type is not well defined ( type =\""<< type <<"\" ). Must be not empty and must be with a rooted namespace.", type.size() && type[0]==':' );
        hasAttributeType = true;
    }

    // buildMode
    bool hasAttributeBuildMode = false;
    std::string buildMode = "";
    if ( _cfgElement->hasAttribute( OBJECT_BUILD_MODE ) )
    {
        buildMode = _cfgElement->getExistingAttributeValue( OBJECT_BUILD_MODE );
        SLM_ASSERT( "Sorry build mode attribute is an empty string", ! buildMode.empty() );
        // pre-condition
        // OBJECT_BUILD_MODE=BUILD_OBJECT
        // OBJECT_BUILD_MODE=GET_OBJECT
        OSLM_ASSERT( OBJECT_BUILD_MODE << " is not well defined. His value is \"" << buildMode << "\". The value must be equal to \"new\" or \"ref\".",
                buildMode == BUILD_OBJECT ||
                buildMode == GET_OBJECT );
        hasAttributeBuildMode = true;

        // if buildMode is defined, uid must be defined
        SLM_ASSERT("Sorry the attribute \"uid\" must defined in object xml element if the attribute "<< OBJECT_BUILD_MODE <<" is defined.", hasAttributeUid );
    }

    // id
    bool hasAttributeId = false;
    std::string id = "";
    if ( _cfgElement->hasAttribute( "id" ) )
    {
        id = _cfgElement->getExistingAttributeValue( "id" );
        SLM_ASSERT( "Sorry id attribute is an empty string", ! id.empty() );
        hasAttributeId = true;
    }



    // Creation of a new object
    ::fwTools::Object::sptr obj;


    // Test if we must create an object or use it.
    if( hasAttributeBuildMode )
    {
        if ( buildMode == BUILD_OBJECT )
        {
            obj = this->createNewObject( hasAttributeType, type, hasAttributeUid, uid, hasAttributeId, id );
        }
        else // if ( buildMode == GET_OBJECT )
        {
            obj = this->getAlreadyDefinedObject( uid, hasAttributeType, type, hasAttributeId, id );
        }
    }
    else
    {
        obj = this->createNewObject( hasAttributeType, type, hasAttributeUid, uid, hasAttributeId, id );
    }


    // Init object ( and perharps build subObject )
    SLM_ASSERT( "Sorry, this object not support IXMLParser", ::fwServices::support< ::fwServices::IXMLParser >( obj ) );

    ::fwServices::IXMLParser::sptr objParserSrv = ::fwServices::get< ::fwServices::IXMLParser >( obj );
    objParserSrv->setConfiguration( _cfgElement ) ;
    objParserSrv->configure() ;
    objParserSrv->start() ;
    objParserSrv->update() ;
    objParserSrv->stop() ;

    // unregister call stop before
    ::fwServices::OSR::unregisterService( objParserSrv );

    // Attaching a configuring services
    this->addServicesToObjectFromCfgElem( obj, _cfgElement );

    return obj ;
}

//-----------------------------------------------------------------------------

void ConfigTemplateManager::addServicesToObjectFromCfgElem( ::fwTools::Object::sptr _obj, ::fwRuntime::ConfigurationElement::sptr _cfgElement )
{
    for(    ::fwRuntime::ConfigurationElementContainer::Iterator iter = _cfgElement->begin() ;
            iter != _cfgElement->end() ;
            ++iter )
    {
        if( (*iter)->getName() == "service" )
        {
            ::fwServices::IService::sptr service = this->add( _obj , (*iter) ) ;
        }
        else if ( (*iter)->getName() == "serviceList" )
        {
            this->addServicesToObjectFromCfgElem( _obj, (*iter) );
        }
    }
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr ConfigTemplateManager::createNewObject( bool _hasAttributeType,
        const std::string & _type,
        bool _hasAttributeUid,
        const std::string & _uid,
        bool _hasAttributeId,
        const std::string & _id )
{

    SLM_ASSERT( "Sorry xml element \"object\" must have type attribute defined.", _hasAttributeType );

    // Building object structure
    ::boost::shared_ptr< ::fwRuntime::Extension > ext = ::fwRuntime::findExtension( _type ) ;
    if( ext )
    {
        assert( ext->getPoint() == ::fwCore::getClassname< ::fwTools::Object >() );
        ext->getBundle()->start(); // start dll to retrieve proxy and register object
    }

    ::fwTools::Object::sptr obj = ::fwTools::Factory::New( _type ) ;
    OSLM_ASSERT( "Sorry ::fwTools::Factory failed to building object : "<< _type, obj );

    // Test and Set UID
    if( _hasAttributeUid )
    {
        OSLM_ASSERT( "Object has already an UID.\n"<< "Can't assign UID: " <<_uid, !obj->hasID() ) ;
        OSLM_FATAL_IF("UID " << _uid << " already exists", ::fwTools::fwID::exist( _uid   ) );
        obj->setID( _uid ) ;
    }

    // Test and Set UID
    if( _hasAttributeId )
    {
        obj->setName( _id ) ;
    }

    return obj;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr ConfigTemplateManager::getAlreadyDefinedObject(
        const std::string & _uid,
        bool _hasAttributeType,
        const std::string & _type,
        bool _hasAttributeId,
        const std::string & _id )
{
    ::fwTools::Object::sptr obj;

    OSLM_ASSERT( "Sorry, not found the object uid : \""<< _uid <<"\" in created object list.", ::fwTools::fwID::exist( _uid ) );
    obj = ::fwTools::fwID::getObject( _uid  );

    // Test type
    OSLM_ASSERT( "Sorry, type of object is defined ( "<< _type <<" ), but not correspond to the object associated ( "<< obj->getClassname() <<" ) to this uid ( "<< _uid <<" ).",  !_hasAttributeType || _type == obj->getClassname() );

    // Test id
    OSLM_ASSERT( "Sorry, id of object is defined ( "<< _id <<" ), but not correspond to the name of object associated ( "<< obj->getName() <<" ) to this uid ( "<< _uid <<" ).",  !_hasAttributeType || _id == obj->getName() );

    return obj;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr ConfigTemplateManager::add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt )
{
    OSLM_ASSERT("ConfigurationElement node name must be \"service\" not "<<_elt->getName(), _elt->getName() == "service" ) ;
    SLM_ASSERT("Attribute \"type\" is missing", _elt->hasAttribute("type") ) ;

    ::fwServices::IService::sptr service ;
    // Search for service type
    std::string serviceType = _elt->getExistingAttributeValue("type") ;
    // Search for implementation type : use default if none specified
    std::string implementationType;
    if(_elt->hasAttribute("implementation") )
    {
        implementationType = _elt->getExistingAttributeValue("implementation");
    }
    else
    {
        SLM_FATAL("ACH => not tolerated  today ?");
        implementationType = ::fwServices::getDefaultImplementationIds( obj , serviceType );
    }

    // Add service with possible id
    if( _elt->hasAttribute("uid")  )
    {
        service = this->add( obj , serviceType , implementationType , _elt->getExistingAttributeValue("uid") );
    }
    else
    {
        service =  this->add( obj , serviceType , implementationType )  ;
    }
    OSLM_ASSERT("Instantiation service "<<implementationType<<" failed on " << obj->getClassname() << " object", service);

    m_createdServices.push_back( service );

    // Search for configuration : inline or offline
    ::fwRuntime::ConfigurationElement::sptr cfg;
    SLM_ASSERT( "Param config not supported today.", ! _elt->hasAttribute("config") );
    cfg =  _elt;

    // Set configuration
    service->setConfiguration( cfg ) ;
    // Configure
    service->configure();

    // Standard communication management
    SLM_ASSERT("autoComChannel attribute missing in service "<< service->getClassname(), _elt->hasAttribute("autoComChannel"));

    std::string autoComChannel = _elt->getExistingAttributeValue("autoComChannel");
    SLM_ASSERT("wrong autoComChannel definition", autoComChannel=="yes" || autoComChannel=="no");
    if(autoComChannel=="yes")
    {
        ::fwServices::ComChannelService::sptr comChannel = ::fwServices::registerCommunicationChannel( obj , service);
        m_createdServices.push_back( comChannel );

        // Add priority for the new comChannel if defined, otherwise the default value is 0.5
        if( _elt->hasAttribute("priority"))
        {
            std::string priorityStr = _elt->getExistingAttributeValue("priority");
            double priority = ::boost::lexical_cast< double >( priorityStr );
            if(priority < 0.0) priority = 0.0;
            if(priority > 1.0) priority = 1.0;
            comChannel->setPriority(priority);
        }
        comChannel->start();
        m_startedServices.push_back( comChannel );
    }

    // Recursive attachment of possibly present subservices
    for( ::fwRuntime::ConfigurationElementContainer::Iterator iter = cfg->begin() ; iter != cfg->end() ; ++iter )
    {
        SLM_ASSERT( "Service of service is not supported.", (*iter)->getName() != "service" );
    }

    // Return
    return service ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr ConfigTemplateManager::add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId,  std::string uid )
{
    IService::sptr srv = ::fwServices::ServiceFactoryRegistry::getDefault()->create( serviceId, _implementationId );
    ::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , srv );

    if ( ! uid.empty() )
    {
        SLM_ASSERT( "Sorry, service has already an fwId.", !srv->hasID() );
        srv->setID( uid ) ;
    }

    return srv;
}

//------------------------------------------------------------------------------

void ConfigTemplateManager::start( ::fwRuntime::ConfigurationElement::sptr _elt )
{
    for( ::fwRuntime::ConfigurationElementContainer::Iterator iter = _elt->begin() ; iter != _elt->end() ; ++iter )
    {
        if( (*iter)->getName() == "start" )
        {
            SLM_ERROR_IF("Sorry the attribute \"type\" for element start is deprecated. You must specify the service uid that you want start.", (*iter)->hasAttribute("type") );
            if ( (*iter)->hasAttribute("type") )
            {
                std::string serviceTypeToStart = (*iter)->getExistingAttributeValue("type") ;
                std::vector< ::fwServices::IService::sptr > servicesToStart = getServices( serviceTypeToStart );
                OSLM_FATAL_IF("Configuration : element " << serviceTypeToStart << " not found", servicesToStart.empty() );
                std::vector< ::fwServices::IService::sptr >::iterator iter = servicesToStart.begin() ;
                for( ; iter != servicesToStart.end() ; ++iter )
                {
                    (*iter)->start();
                    m_startedServices.push_back( *iter );
                }
            }
            else
            {
                SLM_ASSERT("Sorry, the attribute start is required for element start.", (*iter)->hasAttribute("uid") );
                std::string uid = (*iter)->getExistingAttributeValue("uid") ;
                OSLM_FATAL_IF("Configuration : element " << uid << " not found", ! ::fwServices::has(uid));
                ::fwServices::IService::sptr srv = ::fwServices::get(uid);
                srv->start();
                m_startedServices.push_back( srv );
            }
        }
    }
}

//------------------------------------------------------------------------------

}

