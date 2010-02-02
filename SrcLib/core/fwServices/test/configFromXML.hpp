/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICES_TEST_CONFIGFROMXML_HPP_
#define FWSERVICES_TEST_CONFIGFROMXML_HPP_
#include <iostream>
#include <sstream>
#include <vector>
#include <fwTools/UUID.hpp>
#include <boost/shared_ptr.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwCore/base.hpp>
#include <fwTools/Object.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Factory.hpp>
#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include "serviceCreatorAccessor.hpp"
#include "xmlxsd.hpp"


::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildProcessObjectTest1()
{
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
	cfg->setAttributeValue( "id" , "test") ;
	cfg->setAttributeValue( "type" , "ProcessObject") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > inputs = cfg->addConfigurationElement("inputs");
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = inputs->addConfigurationElement("object");
	objA->setAttributeValue( "id" , "OBJ0A") ;
	objA->setAttributeValue( "type" , "Image") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > outputs = cfg->addConfigurationElement("outputs");
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service = cfg->addConfigurationElement("service");
	service->setAttributeValue( "type" , "::ITestService" ) ;
	return cfg ;
}
::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildCompositeTest1()
{
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
	cfg->setAttributeValue( "id" , "test") ;
	cfg->setAttributeValue( "type" , "Composite") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = cfg->addConfigurationElement("object");
	objA->setAttributeValue( "id" , "OBJ0A") ;
	objA->setAttributeValue( "type" , "Image") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objB = cfg->addConfigurationElement("object");
	objB->setAttributeValue( "id" , "OBJ0B") ;
	objB->setAttributeValue( "type" , "Video") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service = cfg->addConfigurationElement("service");
	service->setAttributeValue( "type" , "::ITestService" ) ;
	return cfg ;
}
::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildCompositeTest2()
{
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
	cfg->setAttributeValue( "id" , "test") ;
	cfg->setAttributeValue( "type" , "Composite") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = cfg->addConfigurationElement("object");
	objA->setAttributeValue( "id" , "OBJ0A") ;
	objA->setAttributeValue( "type" , "Image") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objB = cfg->addConfigurationElement("object");
	objB->setAttributeValue( "id" , "OBJ0B") ;
	objB->setAttributeValue( "type" , "Video") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service = cfg->addConfigurationElement("service");
	service->setAttributeValue( "uid" , "myTestService1" ) ;
	service->setAttributeValue( "type" , "::ITestService" ) ;
	return cfg ;
}
::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildCompositeTest3()
{
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
	cfg->setAttributeValue( "id" , "test") ;
	cfg->setAttributeValue( "type" , "Composite") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = cfg->addConfigurationElement("object");
	objA->setAttributeValue( "id" , "OBJ0A") ;
	objA->setAttributeValue( "type" , "Image") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objB = cfg->addConfigurationElement("object");
	objB->setAttributeValue( "id" , "OBJ0B") ;
	objB->setAttributeValue( "type" , "Video") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
	serviceA->setAttributeValue( "uid" , "myTestService1" ) ;
	serviceA->setAttributeValue( "type" , "::ITestService" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
	serviceB->setAttributeValue( "uid" , "myTestService2" ) ;
	serviceB->setAttributeValue( "type" , "::ITestService" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > start = cfg->addConfigurationElement("start");
	start->setAttributeValue( "type" , "::ITestService" ) ;
	
	return cfg ;
}
::boost::shared_ptr< ::fwRuntime::ConfigurationElement > buildCompositeTest4()
{
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
	cfg->setAttributeValue( "id" , "test") ;
	cfg->setAttributeValue( "type" , "Composite") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = cfg->addConfigurationElement("object");
	objA->setAttributeValue( "id" , "OBJ0A") ;
	objA->setAttributeValue( "type" , "Image") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objB = cfg->addConfigurationElement("object");
	objB->setAttributeValue( "id" , "OBJ0B") ;
	objB->setAttributeValue( "type" , "Video") ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
	serviceA->setAttributeValue( "uid" , "myTestService1" ) ;
	serviceA->setAttributeValue( "type" , "::ITestService" ) ;
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
	serviceB->setAttributeValue( "uid" , "myTestService2" ) ;
	serviceB->setAttributeValue( "type" , "::ITestService" ) ;
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

int testConfigFromXML1()
{
	// ProcessObject configuration
	{
		::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _poCfgElement = buildProcessObjectTest1() ;
		toFile(toXml(_poCfgElement),"configPo.xml") ;

		::boost::shared_ptr< ::fwTools::Object > obj = ::fwServices::New(_poCfgElement );
		OSLM_INFO( "Object should be of type ProcessObject " << ( obj->className() == "ProcessObject" ? "OK" : "NOK" ) );
		OSLM_INFO( "Object should has service of type ITestService : " << ( ::fwServices::has( obj , "::ITestService" ) ? "OK" : "NOK" ) );
		assert( ::fwServices::has( obj , "::ITestService" ) ) ;
		::boost::shared_ptr< ::fwData::ProcessObject > po = ::boost::dynamic_pointer_cast< ::fwData::ProcessObject >( obj ) ;
		assert( po ) ;
		assert( po->getInput("OBJ0A") ) ;
		OSLM_INFO( "ProcessObject should has input OBJ0A of type Image : " << ( po->getInput("OBJ0A")->className() == "Image" ? "OK" : "NOK" ) );
	}
	// Composite configuration
	{
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement = buildCompositeTest1() ;
	toFile(toXml(_compoCfgElement),"configComp.xml") ;
	::boost::shared_ptr< ::fwTools::Object > obj = ::fwServices::New(_compoCfgElement );
	OSLM_INFO( "Object should be of type Composite " << ( obj->className() == "Composite" ? "OK" : "NOK" ) );
	OSLM_INFO( "Object should has service of type ITestService : " << ( ::fwServices::has( obj , "::ITestService" ) ? "OK" : "NOK" ) );
	assert( ::fwServices::has( obj , "::ITestService" ) ) ;
	::boost::shared_ptr< ::fwData::Composite > compo = ::boost::dynamic_pointer_cast< ::fwData::Composite >( obj ) ;
	assert( compo ) ;
	assert( compo->getRefMap().find("OBJ0A") != compo->getRefMap().end() ) ;
	OSLM_INFO( "CompositeObject should has an object OBJ0A of type Image : " << ( compo->getRefMap()["OBJ0A"]->className() == "Image" ? "OK" : "NOK" ) );
	}
	
    return 1 ;
}

int testConfigFromXML2()
{
	// Composite configuration
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement1 = buildCompositeTest1() ;
	toFile(toXml(_compoCfgElement1),"configComp1.xml") ;
	::boost::shared_ptr< ::fwTools::Object > obj = ::fwServices::New(_compoCfgElement1 );
	assert( ::fwServices::has( obj , "::ITestService" ) ) ;
	::fwServices::get( obj , "::ITestService" )->start();

	// Composite configuration
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement2 = buildCompositeTest2() ;
	toFile(toXml(_compoCfgElement2),"configComp2.xml") ;
	obj = ::fwServices::New(_compoCfgElement2 );
	assert( ::fwServices::has( obj , "::ITestService" ) ) ;
	::fwServices::get( obj , "::ITestService" , "myTestService1")->start();
	::fwServices::get( "myTestService1" )->start();	
	
    return 1 ;
}

int testConfigFromXML3()
{
	// Composite configuration
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement1 = buildCompositeTest3() ;
	toFile(toXml(_compoCfgElement1),"configComp3.xml") ;
	::boost::shared_ptr< ::fwTools::Object > obj = ::fwServices::New(_compoCfgElement1 );
	fwServices::start( obj , _compoCfgElement1 ) ;

    return 1 ;
}

int testConfigFromXML4()
{
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > _compoCfgElement2 = buildCompositeTest4() ;
	toFile(toXml(_compoCfgElement2),"configComp4.xml") ;
	::boost::shared_ptr< ::fwTools::Object > obj = ::fwServices::New(_compoCfgElement2 );
	fwServices::start( obj , _compoCfgElement2 ) ;
	fwServices::update( obj , _compoCfgElement2 ) ;	
	fwServices::stop( obj , _compoCfgElement2 ) ;	

    return 1 ;
}


#endif /*XMLXSD_HPP_*/
