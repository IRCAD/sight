/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <fwServices/registry/ServiceFactory.hpp>

#include "TestService.hpp"
#include "ObjectServiceTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ObjectServiceTest );

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

void ObjectServiceTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ObjectServiceTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ObjectServiceTest::swapTest()
{
    const std::string srvType("::fwServices::ut::TestService");
    const std::string srvImplementation("::fwServices::ut::TestServiceImplementation");

    ::fwData::Integer::sptr objSrv1 = ::fwData::Integer::New();
    ::fwData::Float::sptr objSrv2   = ::fwData::Float::New();

    ::fwServices::IService::sptr service;
    service = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation );

    ::fwServices::registry::ObjectService osr;

    osr.registerService(objSrv1, service);

    typedef std::vector< ::fwServices::ut::TestService::sptr > ServiceVectorType;
    ServiceVectorType servicesByTemplateTypeAndObj1 = osr.getServices< ::fwServices::ut::TestService >( objSrv1 );
    ServiceVectorType servicesByTemplateTypeAndObj2 = osr.getServices< ::fwServices::ut::TestService >( objSrv2 );

    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByTemplateTypeAndObj1.size());
    CPPUNIT_ASSERT_EQUAL(size_t(0), servicesByTemplateTypeAndObj2.size());
    ::fwServices::IService::sptr osrSrv = servicesByTemplateTypeAndObj1[0];
    CPPUNIT_ASSERT_EQUAL(service, osrSrv);

    osr.swapService(objSrv2, service);

    CPPUNIT_ASSERT(objSrv2 == service->getObject());

    servicesByTemplateTypeAndObj1 = osr.getServices< ::fwServices::ut::TestService >( objSrv1 );
    servicesByTemplateTypeAndObj2 = osr.getServices< ::fwServices::ut::TestService >( objSrv2 );

    CPPUNIT_ASSERT_EQUAL(size_t(0), servicesByTemplateTypeAndObj1.size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByTemplateTypeAndObj2.size());
    osrSrv = servicesByTemplateTypeAndObj2[0];
    CPPUNIT_ASSERT_EQUAL(service, osrSrv);

    osr.unregisterService(service);
}

//------------------------------------------------------------------------------

void ObjectServiceTest::registerTest()
{
    const std::string srvType("::fwServices::ut::TestService");
    const std::string srvImplementation("::fwServices::ut::TestServiceImplementation");

    ::fwData::Integer::sptr obj = ::fwData::Integer::New();

    ::fwServices::IService::sptr service;
    service = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation );

    ::fwServices::registry::ObjectService osr;

    osr.registerService(obj, service);
    CPPUNIT_ASSERT(osr.has(obj, srvType) );

    typedef ::fwServices::registry::ObjectService::ServiceVectorType ServiceVectorType;
    ServiceVectorType servicesByType       = osr.getServices( srvType );
    ServiceVectorType servicesByObjAndType = osr.getServices( obj, srvType );
    typedef std::vector< ::fwServices::ut::TestService::sptr > TestServiceVectorType;
    TestServiceVectorType servicesByTemplateType       = osr.getServices< ::fwServices::ut::TestService >( );
    TestServiceVectorType servicesByTemplateTypeAndObj = osr.getServices< ::fwServices::ut::TestService >( obj );


    CPPUNIT_ASSERT(servicesByType == servicesByObjAndType);
    CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateTypeAndObj.begin()));

    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByType.size());
    typedef ::fwServices::registry::ObjectService::ObjectVectorType ObjectVectorType;
    ::fwServices::IService::sptr osrService = servicesByType[0];
    CPPUNIT_ASSERT_EQUAL(service, osrService);

    ObjectVectorType objects = osr.getObjects();
    CPPUNIT_ASSERT_EQUAL(size_t(1), objects.size());
    ::fwData::Object::sptr osrObject = objects[0];
    CPPUNIT_ASSERT(obj == osrObject);

    osr.unregisterService(service);
    CPPUNIT_ASSERT( osr.has(obj, srvType) == false );
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT( servicesByType.empty() );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
