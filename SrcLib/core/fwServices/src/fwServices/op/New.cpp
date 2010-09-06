/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/UUID.hpp>
#include <fwTools/Factory.hpp>
#include <fwCore/spyLog.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/Base.hpp"
#include "fwServices/op/New.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

bool verifyObjectXmlElement( ::fwRuntime::ConfigurationElement::sptr _cfgElement )
{
    bool configFileIsOk = true;

    for(    ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin();
            configEltIter != _cfgElement->end();
            ++configEltIter )
    {
        if( !(  (*configEltIter)->getName() == "service" ||
                (*configEltIter)->getName() == "serviceList" ||
                (*configEltIter)->getName() == "start" ||
                (*configEltIter)->getName() == "stop" ||
                (*configEltIter)->getName() == "update" ||
                (*configEltIter)->getName() == "item" ) )
        {
            OSLM_ERROR( "Sorry, the xml element : "<< (*configEltIter)->getName() <<" is not supported.");
            configFileIsOk = false;
        }
    }

    return configFileIsOk;
}


//-----------------------------------------------------------------------------

::fwTools::Object::sptr createNewObject( bool _hasAttributeType,
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
        OSLM_ASSERT( "Object has already an UID.\n"<< "Can't assign UID: " <<_uid, !obj->hasUUID() ) ;
        OSLM_FATAL_IF("UID " << _uid << " already exists", ::fwTools::UUID::exist( _uid , ::fwTools::UUID::SIMPLE ) );
        obj->setUUID( _uid ) ;
    }

    // Test and Set UID
    if( _hasAttributeId )
    {
        obj->setName( _id ) ;
    }

    return obj;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr getAlreadyDefinedObject(
        const std::string & _uid,
        bool _hasAttributeType,
        const std::string & _type,
        bool _hasAttributeId,
        const std::string & _id )
{
    ::fwTools::Object::sptr obj;

    OSLM_ASSERT( "Sorry, not found the object uid : \""<< _uid <<"\" in created object list.", ::fwTools::UUID::exist( _uid , ::fwTools::UUID::SIMPLE ) );
    obj = ::fwTools::UUID::get< ::fwTools::Object >( _uid , ::fwTools::UUID::SIMPLE );

    // Test type
    OSLM_ASSERT( "Sorry, type of object is defined ( "<< _type <<" ), but not correspond to the object associated ( "<< obj->getClassname() <<" ) to this uid ( "<< _uid <<" ).",  !_hasAttributeType || _type == obj->getClassname() );

    // Test id
    OSLM_ASSERT( "Sorry, id of object is defined ( "<< _id <<" ), but not correspond to the name of object associated ( "<< obj->getName() <<" ) to this uid ( "<< _uid <<" ).",  !_hasAttributeType || _id == obj->getName() );

    return obj;
}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr New( ::fwRuntime::ConfigurationElement::sptr _cfgElement )
{
    OSLM_ASSERT("Sorry method New( cfg ) works only on an object xml config, here xml element is : " << _cfgElement->getName(), _cfgElement->getName() == "object" );
    SLM_ASSERT("Sorry, some xml sub-elements of object are no correct", verifyObjectXmlElement( _cfgElement ) );
    OSLM_ASSERT("Sorry, attribute \"id\" ( val = "<< ( _cfgElement->hasAttribute( "id" ) ? _cfgElement->getExistingAttributeValue("id") : "" ) <<" ) of an object xml elements his deprecated.", ! _cfgElement->hasAttribute( "id" ) );


    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT = "new";
    const std::string GET_OBJECT = "ref";
    const std::string GET_OR_BUILD_OBJECT = "auto";



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
        // OBJECT_BUILD_MODE=GET_OR_BUILD_OBJECT
        OSLM_ASSERT( OBJECT_BUILD_MODE << " is not well defined. His value is \"" << buildMode << "\". The value must be equal to \"notExist\", \"mustExist\" or \"canExist\".",
                buildMode == BUILD_OBJECT ||
                buildMode == GET_OBJECT ||
                buildMode == GET_OR_BUILD_OBJECT );
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
        // If get or build mode, test uid and adapt mode
        if ( buildMode == GET_OR_BUILD_OBJECT )
        {
            if ( ::fwTools::UUID::exist( uid , ::fwTools::UUID::SIMPLE ) )
            {
                buildMode = GET_OBJECT;
            }
            else
            {
                buildMode == BUILD_OBJECT;
            }
        }


        if ( buildMode == BUILD_OBJECT )
        {
            obj = createNewObject( hasAttributeType, type, hasAttributeUid, uid, hasAttributeId, id );
        }
        else // if ( buildMode == GET_OBJECT )
        {
            obj = getAlreadyDefinedObject( uid, hasAttributeType, type, hasAttributeId, id );
        }
    }
    else
    {
        obj = createNewObject( hasAttributeType, type, hasAttributeUid, uid, hasAttributeId, id );
    }





    // Retrieve IXML parser in charge of initializing (data) obj content
    if( ::fwServices::support< ::fwServices::IXMLParser >( obj ) )
    {
        ::fwServices::IXMLParser::sptr objParserSrv = ::fwServices::get< ::fwServices::IXMLParser >( obj );
        objParserSrv->setConfiguration( _cfgElement ) ;
        objParserSrv->configure() ;
        objParserSrv->start() ;
        objParserSrv->update() ;
        // unregister call stop before
        ::fwServices::OSR::unregisterService( objParserSrv );
    }

    // Attaching a configuring services
    ::fwServices::addServicesToObjectFromCfgElem( obj, _cfgElement );

    return obj ;
}

//-----------------------------------------------------------------------------

void addServicesToObjectFromCfgElem( ::fwTools::Object::sptr _obj, ::fwRuntime::ConfigurationElement::sptr _cfgElement )
{
    for(    ::fwRuntime::ConfigurationElementContainer::Iterator iter = _cfgElement->begin() ;
        iter != _cfgElement->end() ;
        ++iter )
    {
        if( (*iter)->getName() == "service" )
        {
            ::fwServices::IService::sptr service = ::fwServices::add( _obj , (*iter) ) ;
            assert( service );
        }
        else if ( (*iter)->getName() == "serviceList" )
        {
            ::fwServices::addServicesToObjectFromCfgElem( _obj, (*iter) );
        }
    }
}

//-----------------------------------------------------------------------------



}

