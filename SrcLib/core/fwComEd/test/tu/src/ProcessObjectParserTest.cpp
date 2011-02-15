/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>

#include <fwData/ProcessObject.hpp>
#include <fwData/Image.hpp>
#include <fwData/Video.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>

#include <fwComEd/ProcessObjectEditor.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/objectConfigurationParser.hpp>

#include "ProcessObjectParserTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ProcessObjectParserTest );

//------------------------------------------------------------------------------

void ProcessObjectParserTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ProcessObjectParserTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ProcessObjectParserTest::testBuildProcessObject()
{
    /* FIXME : update the config/parsing for a po
    const std::string IMAGE1ID = "OBJ0A";
    const std::string VIDEOID = "OBJ0B";
    const std::string IMAGE2ID = "OBJ0C";

    ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > config = buildConfig();
    ::fwData::ProcessObject::sptr po = ::fwServices::New< ::fwData::ProcessObject >(config );

    CPPUNIT_ASSERT(po);
    CPPUNIT_ASSERT(po->getInput(IMAGE1ID));
    CPPUNIT_ASSERT(po->getInput(VIDEOID));
    CPPUNIT_ASSERT(po->getOutput(IMAGE2ID));

    ::fwData::Image::sptr image1 = po->getInput< ::fwData::Image >(IMAGE1ID);
    CPPUNIT_ASSERT_EQUAL(IMAGE1ID, image1->getID());

    ::fwData::Video::sptr video = po->getInput< ::fwData::Video >(VIDEOID);
    CPPUNIT_ASSERT_EQUAL(VIDEOID, video->getID());

    ::fwData::Image::sptr image2 = po->getOutput< ::fwData::Image >(IMAGE2ID);
    CPPUNIT_ASSERT_EQUAL(IMAGE2ID, image2->getID());

    CPPUNIT_ASSERT(::fwServices::ObjectServiceRegistry::has(po, "::TestService"));
    CPPUNIT_ASSERT(::fwServices::ObjectServiceRegistry::has(po, "::fwServices::IEditionService"));
    CPPUNIT_ASSERT(::fwServices::ObjectServiceRegistry::has(image1, "::TestService"));

    ::TestService::sptr servicePo;

    servicePo = ::fwServices::get< ::TestService >(po);
    CPPUNIT_ASSERT(servicePo);

    fwServices::start( config ) ;
    CPPUNIT_ASSERT(servicePo->isStarted());

    fwServices::update( config ) ;
    CPPUNIT_ASSERT(servicePo->getIsUpdated());

    fwServices::stop( config ) ;
    CPPUNIT_ASSERT(servicePo->isStopped());
    */
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwRuntime::ConfigurationElement > ProcessObjectParserTest::buildConfig()
{
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
    cfg->setAttributeValue( "uid" , "processObjectUUID") ;
    cfg->setAttributeValue( "id" , "processObjectUUID") ;
    cfg->setAttributeValue( "type" , "::fwData::ProcessObject") ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > inputs = cfg->addConfigurationElement("inputs");

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = inputs->addConfigurationElement("object");
    objA->setAttributeValue( "uid" , "OBJ0A") ;
    objA->setAttributeValue( "id" , "OBJ0A") ;
    objA->setAttributeValue( "type" , "::fwData::Image") ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceObjA = objA->addConfigurationElement("service");
    serviceObjA->setAttributeValue( "uid" , "myTestServiceObjA" ) ;
    serviceObjA->setAttributeValue( "type" , "::TestService" ) ;
    serviceObjA->setAttributeValue( "implementation" , "::TestServiceImplementation" ) ;
    serviceObjA->setAttributeValue( "autoComChannel" , "no" ) ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objB = inputs->addConfigurationElement("object");
    objB->setAttributeValue( "uid" , "OBJ0B") ;
    objB->setAttributeValue( "id" , "OBJ0B") ;
    objB->setAttributeValue( "type" , "::fwData::Video") ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > outputs = cfg->addConfigurationElement("outputs");
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objC = outputs->addConfigurationElement("object");
    objC->setAttributeValue( "uid" , "OBJ0C") ;
    objC->setAttributeValue( "id" , "OBJ0C") ;
    objC->setAttributeValue( "type" , "::fwData::Image") ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service = cfg->addConfigurationElement("service");
    service->setAttributeValue( "uid" , "myTestService1" ) ;
    service->setAttributeValue( "type" , "::TestService" ) ;
    service->setAttributeValue( "implementation" , "::TestServiceImplementation" ) ;
    service->setAttributeValue( "autoComChannel" , "no" ) ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > start = cfg->addConfigurationElement("start");
    start->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update = cfg->addConfigurationElement("update");
    update->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stop = cfg->addConfigurationElement("stop");
    stop->setAttributeValue( "uid" , "myTestService1" ) ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid" , "myTestService2" ) ;
    service2->setAttributeValue( "type" , "::fwServices::IEditionService" ) ;
    service2->setAttributeValue( "implementation" , "::fwComEd::ProcessObjectEditor" ) ;
    service2->setAttributeValue( "autoComChannel" , "no" ) ;

    return cfg ;
}

//------------------------------------------------------------------------------
