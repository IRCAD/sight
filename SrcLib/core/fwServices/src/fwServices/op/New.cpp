/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/UUID.hpp>
#include <fwTools/Factory.hpp>

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

::fwTools::Object::sptr New(::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    assert( _cfgElement->hasAttribute("type")) ;
    std::string _name = _cfgElement->getExistingAttributeValue("type") ;

    assert( _name.size() && _name[0]==':'); // must use rooted namespace

    // Building object structure
     ::boost::shared_ptr< ::fwRuntime::Extension > ext = ::fwRuntime::findExtension(_name) ;
    if( ext )
    {
        assert( ext->getPoint() == ::fwCore::getClassname< ::fwTools::Object >() );
        ext->getBundle()->start(); // start dll to retrieve proxy and register object
    }

    ::fwTools::Object::sptr obj = ::fwTools::Factory::New( _name ) ;
    assert(obj);
    if( _cfgElement->hasAttribute("id"))
    {
            obj->setName( _cfgElement->getExistingAttributeValue("id") );
    }
    if( _cfgElement->hasAttribute("uid"))
    {
        assert( !obj->hasUUID() ) ;
        if( ::fwTools::UUID::exist( _cfgElement->getExistingAttributeValue("uid") , ::fwTools::UUID::SIMPLE) )
        {
            OSLM_FATAL("ID " << _cfgElement->getExistingAttributeValue("uid") << " already exists");
        }
        else
        {
            obj->setUUID( _cfgElement->getExistingAttributeValue("uid") ) ;
        }
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

        //::fwServices::erase< ::fwServices::IXMLParser >( obj ) ;
        /*
        ::fwServices::get< ::fwServices::IXMLParser >( obj )->setConfiguration( _cfgElement ) ;
        ::fwServices::get< ::fwServices::IXMLParser >( obj )->configure() ;
        ::fwServices::get< ::fwServices::IXMLParser >( obj )->start() ;
        ::fwServices::get< ::fwServices::IXMLParser >( obj )->update() ;
        ::fwServices::get< ::fwServices::IXMLParser >( obj )->stop() ;
        ::fwServices::erase< ::fwServices::IXMLParser >( obj ) ;
        */

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

