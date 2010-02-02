/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICETEST_OBSERVATIONTEST1_HPP_
#define FWSERVICETEST_OBSERVATIONTEST1_HPP_

#include <iostream>
#include <sstream>
#include <vector>
#include <fwTools/UUID.hpp>
#include <boost/shared_ptr.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ComChannelService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwComEd/CompositeEditor.hpp>
#include <fwComEd/CompositeMsg.hpp>
#include <fwCore/base.hpp>
#include <fwData/Object.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Composite.hpp>
#include <fwTools/Factory.hpp>
#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include "testService.hpp"
#include "xmlxsd.hpp"

REGISTER_SERVICE( ::fwServices::ICommunication , ::fwServices::ComChannelService , ::fwData::Object ) ;
REGISTER_SERVICE( ::fwServices::IEditionService , ::fwComEd::CompositeEditor, ::fwData::Composite         ) ;
REGISTER_SERVICE( ::fwServices::IEditionService , ::fwServices::IEditionService, ::fwData::Object         ) ;

::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildConfigTest1()
{
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
	cfg->setAttributeValue( "id" , "test") ;
	cfg->setAttributeValue( "uid" , "test") ;
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
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceC = cfg->addConfigurationElement("service");
	serviceC->setAttributeValue( "uid" , "myComChannel1" ) ;
	serviceC->setAttributeValue( "type" , "::fwServices::ICommunication" ) ;
	serviceC->addConfigurationElement("source")->setValue("test");
	serviceC->addConfigurationElement("target")->setValue("myTestService1");
	
	//// START
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
	startA->setAttributeValue( "uid" , "myTestService1" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startB = cfg->addConfigurationElement("start");
	startB->setAttributeValue( "uid" , "myTestService2" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startC = cfg->addConfigurationElement("start");
	startC->setAttributeValue( "uid" , "myComChannel1" ) ;

	//// UPDATE
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update = cfg->addConfigurationElement("update");
	update->setAttributeValue( "uid" , "myTestService1" ) ;
	
	//// STOP	
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopA = cfg->addConfigurationElement("stop");
	stopA->setAttributeValue( "uid" , "myTestService1" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopB = cfg->addConfigurationElement("stop");
	stopB->setAttributeValue( "uid" , "myTestService2" ) ;
	
	return cfg ;
}

::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildConfigTest2()
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
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceC = cfg->addConfigurationElement("service");
	serviceC->setAttributeValue( "type" , "::fwServices::ICommunication" ) ;
	serviceC->addConfigurationElement("source")->setValue("OBJ0A");
	serviceC->addConfigurationElement("target")->setValue("myTestService1");
	
	//// START
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
	startA->setAttributeValue( "type" , "::fwServices::ICommunication" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startB = cfg->addConfigurationElement("start");
	startB->setAttributeValue( "uid" , "myTestService1" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startC = cfg->addConfigurationElement("start");
	startC->setAttributeValue( "uid" , "myTestService2" ) ;

	//// UPDATE
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update = cfg->addConfigurationElement("update");
	update->setAttributeValue( "uid" , "myTestService1" ) ;
	
	//// STOP	
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopA = cfg->addConfigurationElement("stop");
	stopA->setAttributeValue( "uid" , "myTestService1" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopB = cfg->addConfigurationElement("stop");
	stopB->setAttributeValue( "uid" , "myTestService2" ) ;
	
	return cfg ;
}

int test1()
{
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement = buildConfigTest1() ;
	toFile(toXml(_compoCfgElement),"configTest1.xml") ;
	::boost::shared_ptr< ::fwData::Object > obj = ::fwServices::New(_compoCfgElement );

	fwServices::start( obj , _compoCfgElement ) ;
//
//	// Test observation : simulate modification
	OSLM_INFO("Modifying (simulation) root object test" ) ;
	 ::boost::shared_ptr< ::fwServices::IEditionService > editor = ::fwServices::get< fwServices::IEditionService >( obj ) ;
	 ::boost::shared_ptr< ::fwServices::ObjectMsg > msg( new ::fwServices::ObjectMsg(obj) ) ;
	msg->setAllModified() ;
	editor->notify( msg );

//	fwServices::start( obj , _compoCfgElement2 ) ;
//	fwServices::update( obj , _compoCfgElement2 ) ;	
//	fwServices::stop( obj , _compoCfgElement2 ) ;	

    return 1 ;
}

int test2()
{
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement = buildConfigTest2() ;
	toFile(toXml(_compoCfgElement),"configTest2.xml") ;
	::boost::shared_ptr< ::fwData::Object > obj = ::fwServices::New(_compoCfgElement );

	fwServices::start( obj , _compoCfgElement ) ;

	// Test observation : simulate modification
	::boost::shared_ptr< ::fwData::Composite > compo = ::boost::dynamic_pointer_cast< ::fwData::Composite >( obj ) ;
	assert( compo ) ;
	OSLM_INFO("Modifying (simulation) sub object OBJ0A" ) ;

	 ::boost::shared_ptr< ::fwServices::IEditionService > editor = ::fwServices::get< fwServices::IEditionService >( compo->getRefMap()["OBJ0A"] ) ;
	 ::boost::shared_ptr< ::fwServices::ObjectMsg > msg( new ::fwServices::ObjectMsg(compo->getRefMap()["OBJ0A"]) ) ;
	msg->setAllModified() ;
	editor->notify( msg );

	// Stop some services and test observation : FAILED
	fwServices::stop( obj , _compoCfgElement ) ;
	OSLM_INFO("Stopping services and modifying (simulation) sub object OBJ0A : should not result in a manageModification of ITestService" ) ;
	editor->notify( msg );
	// Erase "::ITestService" and test observation : FAILED
	::fwServices::eraseServices( obj , "::ITestService" ) ;
	OSLM_INFO("Erasing services and modifying (simulation) sub object OBJ0A : should not result in a manageModification of ITestService" ) ;
	editor->notify( msg );
	

    return 1 ;
}

#endif /*CONFIGXMLUUIDOBJSERVOBSERVATION_HPP_*/
