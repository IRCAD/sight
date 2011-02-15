/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <fwRuntime/operations.hpp>

#include "fwServices/AppConfigManager.hpp"
#include "fwServices/Base.hpp"
#include "fwServices/op/Get.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/validation/Validator.hpp"
#include "fwServices/registry/ServiceConfig.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

AppConfigManager::AppConfigManager() : m_state ( CONFIG_IS_DESTROYED )
{}

//-----------------------------------------------------------------------------

AppConfigManager::~AppConfigManager()
{
    SLM_ASSERT("Sorry, you must stop the manager first before destroying it.", m_state == CONFIG_IS_DESTROYED );
}

//-----------------------------------------------------------------------------

void AppConfigManager::setConfig( ::fwRuntime::ConfigurationElement::csptr _cfgElem )
{
    m_adaptedConfig = _cfgElem;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr AppConfigManager::getConfigRoot() const{

    return m_configRoot;
}

//-----------------------------------------------------------------------------

void AppConfigManager::create()
{
    SLM_ASSERT("Sorry, manager is already running and you try creating config.", m_state == CONFIG_IS_DESTROYED );

    //SLM_ASSERT("Sorry, the xml that describes extension "<< m_configName <<" is not valid.", ::fwServices::validation::checkObject( m_adaptedConfig ) );

    // Create object and services
    m_configRoot = this->newObject( m_adaptedConfig );

    m_state = CONFIG_IS_CREATED;
}

//-----------------------------------------------------------------------------

void AppConfigManager::startComChannel()
{
    BOOST_FOREACH( ::fwServices::IService::wptr wsrv, m_startedComChannelServices )
            {
        SLM_ASSERT( "Sorry, CTM must start a service, but it is expired", ! wsrv.expired());
        ::fwServices::IService::sptr srv = wsrv.lock();
        OSLM_ASSERT( "Sorry, CTM must start a service ( uid = "<< srv->getID() <<" , classname = "<< srv->getClassname() <<" ), but it is already started", ! srv->isStarted() );
        OSLM_INFO("Start service ( " << srv->getID() << " ) managed by the AppConfigManager.");
        srv->start();
            }
}

//-----------------------------------------------------------------------------

void AppConfigManager::stopComChannel()
{
    BOOST_REVERSE_FOREACH( ::fwServices::IService::wptr wsrv, m_startedComChannelServices )
            {
        SLM_ASSERT( "Sorry, CTM must stop a service, but it is expired", ! wsrv.expired());
        ::fwServices::IService::sptr srv = wsrv.lock();
        OSLM_ASSERT( "Sorry, CTM must stop a service ( uid = "<< srv->getID() <<" , classname = "<< srv->getClassname() <<" ), but it is already stopped", ! srv->isStopped() );
        OSLM_INFO("Stop service ( " << srv->getID() << " ) managed by the AppConfigManager.");
        srv->stop();
            }
    m_startedComChannelServices.clear();
}

//-----------------------------------------------------------------------------

void AppConfigManager::start()
{
    SLM_ASSERT("Sorry, manager is not created and you try starting it.", m_state == CONFIG_IS_CREATED || m_state == CONFIG_IS_STOPPED );

    this->start( m_adaptedConfig ) ;
    m_objectParser->startConfig();
    this->startComChannel();

    m_state = CONFIG_IS_STARTED;
}

//-----------------------------------------------------------------------------

void AppConfigManager::update()
{
    ::fwServices::update( m_adaptedConfig ) ;
    m_objectParser->updateConfig();
}

//-----------------------------------------------------------------------------

void AppConfigManager::launch()
{
    create();
    start();
    update();
}

//-----------------------------------------------------------------------------

void AppConfigManager::stop()
{
    SLM_ASSERT("Sorry, manager is not started and you try stopping it.", m_state == CONFIG_IS_STARTED );

    this->stopComChannel();

    m_objectParser->stopConfig();

    BOOST_REVERSE_FOREACH( ::fwServices::IService::wptr wsrv, m_startedServices )
    {
        SLM_ASSERT( "Sorry, CTM must stop a service, but it is expired", ! wsrv.expired());
        ::fwServices::IService::sptr srv = wsrv.lock();
        OSLM_ASSERT( "Sorry, CTM must stop a service ( uid = "<< srv->getID() <<" , classname = "<< srv->getClassname() <<" ), but it is already stopped", ! srv->isStopped() );
        OSLM_INFO("Stop service ( " << srv->getID() << " ) managed by the AppConfigManager.");
        srv->stop();
    }
    m_startedServices.clear();


    m_state = CONFIG_IS_STOPPED;
    OSLM_INFO( "Parsing OSR after stopping the config : \n" << ::fwServices::OSR::getRegistryInformation() );
}

//-----------------------------------------------------------------------------

void AppConfigManager::destroy()
{
    SLM_ASSERT("Sorry, manager is not stopped and you try destroying it.", m_state == CONFIG_IS_STOPPED || m_state == CONFIG_IS_CREATED );

    m_objectParser->destroyConfig();

    BOOST_REVERSE_FOREACH( ::fwServices::IService::wptr wsrv, m_createdServices )
    {
        SLM_ASSERT( "Sorry, CTM must destroy a service, but it is already expired.", ! wsrv.expired());
        ::fwServices::IService::sptr srv = wsrv.lock();
        OSLM_ASSERT( "Sorry, CTM must destroy a service ( uid = "<< srv->getID() <<" , classname = "<< srv->getClassname() <<" ), but it must be stopped before.", srv->isStopped() );
        OSLM_INFO("Unregister service ( " << srv->getID() << " ) managed by the AppConfigManager.");
        OSR::unregisterService( srv );
    }
    m_createdServices.clear();

    OSLM_INFO( "Parsing OSR after destroying the config : \n" << ::fwServices::OSR::getRegistryInformation() );


    m_objectParser.reset();
    m_adaptedConfig.reset();
    m_configRoot.reset();

    m_state = CONFIG_IS_DESTROYED;
}

//-----------------------------------------------------------------------------

void AppConfigManager::stopAndDestroy()
{
    stop();
    destroy();
}

//-----------------------------------------------------------------------------

//void AppConfigManager::read( ::fwRuntime::ConfigurationElement::csptr _cfgElem )
//{
//    SLM_TRACE_FUNC();
//
//    ::fwRuntime::ConfigurationElement::csptr result;
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
//    BOOST_FOREACH(::fwRuntime::ConfigurationElement::csptr subElem, _cfgElem->getElements())
//    {
//        this->read( subElem );
//    }
//}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr AppConfigManager::newObject( ::fwRuntime::ConfigurationElement::csptr _cfgElement )
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

    //std::string srvImpl = ::fwServices::getDefaultImplementationIds( obj , "::fwServices::IXMLParser" );
    std::string srvImpl = ::fwServices::registry::ServiceFactory::getDefault()->getDefaultImplementationIdFromObjectAndType( obj->getClassname() , "::fwServices::IXMLParser" );
    IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create( "::fwServices::IXMLParser", srvImpl );
    m_objectParser = ::fwServices::IXMLParser::dynamicCast( srv );
    m_objectParser->setObjectConfig( _cfgElement );
    m_objectParser->createConfig( obj );

    // Attaching a configuring services
    this->addServicesToObjectFromCfgElem( obj, _cfgElement );

    return obj ;
}

//-----------------------------------------------------------------------------

void AppConfigManager::addServicesToObjectFromCfgElem( ::fwTools::Object::sptr _obj, ::fwRuntime::ConfigurationElement::csptr _cfgElement )
{
    BOOST_FOREACH( ::fwRuntime::ConfigurationElement::csptr elem, _cfgElement->getElements() )
    {
        if( elem->getName() == "service" )
        {
            ::fwServices::IService::sptr service = this->add( _obj , elem ) ;
        }
        else if ( elem->getName() == "serviceList" )
        {
            this->addServicesToObjectFromCfgElem( _obj, elem );
        }
    }
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr AppConfigManager::createNewObject( bool _hasAttributeType,
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

::fwTools::Object::sptr AppConfigManager::getAlreadyDefinedObject(
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

::fwServices::IService::sptr AppConfigManager::add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::csptr _elt )
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
        // implementationType = ::fwServices::getDefaultImplementationIds( obj , serviceType );
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
    ::fwRuntime::ConfigurationElement::csptr cfg = _elt;
    if( _elt->hasAttribute("config") )
    {
        //cfg = ::fwServices::bundle::findConfigurationForPoint( _elt->getExistingAttributeValue("config") , implementationType );
        cfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( _elt->getExistingAttributeValue("config") , implementationType );
    }
    SLM_ASSERT("Sorry, constCast failed", ::fwRuntime::ConfigurationElement::constCast( cfg ) );

    // Set configuration
    service->setConfiguration( ::fwRuntime::ConfigurationElement::constCast( cfg ) ) ; // ToDo replace by a copy method
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
        //comChannel->start();
        //m_startedServices.push_back( comChannel );
        m_startedComChannelServices.push_back( comChannel );
    }

    // Recursive attachment of possibly present subservices
    for(    ::fwRuntime::ConfigurationElementContainer::Container::const_iterator iter = cfg->getElements().begin();
            iter != cfg->getElements().end();
            ++iter )
    {
        SLM_ASSERT( "Service of service is not supported.", (*iter)->getName() != "service" );
    }

    // Return
    return service ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr AppConfigManager::add( ::fwTools::Object::sptr obj , std::string serviceId , std::string _implementationId,  std::string uid )
{
    IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create( serviceId, _implementationId );
    ::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , srv );

    if ( ! uid.empty() )
    {
        SLM_ASSERT( "Sorry, service has already an fwId.", !srv->hasID() );
        srv->setID( uid ) ;
    }

    return srv;
}

//------------------------------------------------------------------------------

void AppConfigManager::start( ::fwRuntime::ConfigurationElement::csptr _elt )
{
    BOOST_FOREACH( ::fwRuntime::ConfigurationElement::csptr elem, _elt->getElements() )
    {
        if( elem->getName() == "start" )
        {
            SLM_ERROR_IF("Sorry the attribute \"type\" for element start is deprecated. You must specify the service uid that you want start.", elem->hasAttribute("type") );
            if ( elem->hasAttribute("type") )
            {
                std::string serviceTypeToStart = elem->getExistingAttributeValue("type") ;
                std::vector< ::fwServices::IService::sptr > servicesToStart = getServices( serviceTypeToStart );
                OSLM_FATAL_IF("Sorry, try to start services of type " << serviceTypeToStart << ", but this type of service is not found", servicesToStart.empty() );
                std::vector< ::fwServices::IService::sptr >::iterator iter = servicesToStart.begin() ;
                for( ; iter != servicesToStart.end() ; ++iter )
                {
                    (*iter)->start();
                    m_startedServices.push_back( *iter );
                }
            }
            else
            {
                SLM_ASSERT("Sorry, the attribute start is required for element start.", elem->hasAttribute("uid") );
                std::string uid = elem->getExistingAttributeValue("uid") ;
                OSLM_FATAL_IF("Sorry, try to start this service (" << uid << "), but this fwId is not found.", ! ::fwServices::has(uid));
                ::fwServices::IService::sptr srv = ::fwServices::get(uid);
                srv->start();
                m_startedServices.push_back( srv );
            }
        }
    }
}

//------------------------------------------------------------------------------

}

