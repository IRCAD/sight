/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICETEST_CONFIGXMLUUIDOBJSERVOBSERVATION_HPP_
#define FWSERVICETEST_CONFIGXMLUUIDOBJSERVOBSERVATION_HPP_

#include <iostream>
#include <sstream>
#include <vector>
#include <fwTools/UUID.hpp>
#include <boost/shared_ptr.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ComChannelService.hpp>
#include <fwServices/validation/Validator.hpp>
#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Factory.hpp>
#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include "serviceCreatorAccessor.hpp"
#include "xmlxsd.hpp"

REGISTER_SERVICE( ::fwServices::ICommunication , ::fwServices::ComChannelService , ::fwTools::Object ) ;

::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildCompositeTestObjUUID()
{
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
    cfg->setAttributeValue( "uid" , "test") ;
    cfg->setAttributeValue( "id" , "test") ;
    cfg->setAttributeValue( "type" , "Composite") ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = cfg->addConfigurationElement("object");
    objA->setAttributeValue( "uid" , "OBJ0A") ;
    objA->setAttributeValue( "id" , "OBJ0A") ;
    objA->setAttributeValue( "type" , "Image") ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objB = cfg->addConfigurationElement("object");
    objB->setAttributeValue( "uid" , "OBJ0B") ;
    objB->setAttributeValue( "id" , "OBJ0B") ;
    objB->setAttributeValue( "type" , "Video") ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
    serviceA->setAttributeValue( "uid" , "myTestService1" ) ;
    serviceA->setAttributeValue( "type" , "::ITestService" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
    serviceB->setAttributeValue( "uid" , "myTestService2" ) ;
    serviceB->setAttributeValue( "type" , "::ITestService" ) ;
    serviceB->setAttributeValue( "implementation" , "::ImplementationATestService" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
    startA->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startB = cfg->addConfigurationElement("start");
    startB->setAttributeValue( "uid" , "myTestService2" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update = cfg->addConfigurationElement("update");
    update->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopA = cfg->addConfigurationElement("stop");
    stopA->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopB = cfg->addConfigurationElement("stop");
    stopB->setAttributeValue( "uid" , "myTestService2" ) ;
    
    return cfg ;
}

int testConfigUUIDFromXMLObservation1()
{
    ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement = buildCompositeTestObjUUID() ;
    toFile(toXml(_compoCfgElement),"buildCompositeTestObjUUID.xml") ;
    ::boost::shared_ptr< ::fwTools::Object > obj = ::fwServices::New(_compoCfgElement );
    // Test uid
    OSLM_INFO("Object UUID " << obj->getUUID()) ;
    ::boost::shared_ptr< ::fwData::Composite > compo = ::boost::dynamic_pointer_cast< ::fwData::Composite >( obj ) ;
    assert( compo ) ;
    OSLM_INFO("Composite object has OBJ0A whose UUID is " << compo->getRefMap()["OBJ0A"]->getUUID() ) ;
    // Test observation
    
//  fwServices::start( obj , _compoCfgElement2 ) ;
//  fwServices::update( obj , _compoCfgElement2 ) ; 
//  fwServices::stop( obj , _compoCfgElement2 ) ;   

    return 1 ;
}

int testConfigCheck()
{
    ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement = buildCompositeTestObjUUID() ;
    assert( ::fwServices::validation::checkObject( _compoCfgElement , "Composite") ) ;
    toFile(toXml(_compoCfgElement),"testConfigCheck.xml") ;

    return 1 ;
}


#endif /*CONFIGXMLUUIDOBJSERVOBSERVATION_HPP_*/
