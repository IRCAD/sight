/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>
#include <fwData/Vector.hpp>
#include <fwData/String.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwComEd/VectorMsg.hpp>
#include <fwComEd/helper/Vector.hpp>

#include "VectorMsgTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwComEd::ut::VectorMsgTest );


fwServicesRegisterMacro( ::fwComEd::ut::TestService , ::fwComEd::ut::SVectorTest , ::fwData::Vector ) ;

namespace fwComEd
{
namespace ut
{

//------------------------------------------------------------------------------

void VectorMsgTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void VectorMsgTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void VectorMsgTest::basicTest()
{
    ::fwData::String::sptr str1 = ::fwData::String::New("Str1");
    ::fwData::String::sptr str2 = ::fwData::String::New("Str2");

    ::fwComEd::VectorMsg::sptr msg = ::fwComEd::VectorMsg::New();
    msg->appendAddedObject(str1);
    msg->appendAddedObject(str2);

    CPPUNIT_ASSERT(msg->hasEvent(::fwComEd::VectorMsg::ADDED_OBJECTS));

    ::fwData::Vector::sptr vecAddedObj = msg->getAddedObjects();
    CPPUNIT_ASSERT(std::find(vecAddedObj->begin(), vecAddedObj->end(), str1) != vecAddedObj->end());
    CPPUNIT_ASSERT(std::find(vecAddedObj->begin(), vecAddedObj->end(), str2) != vecAddedObj->end());

    ::fwComEd::VectorMsg::sptr msg2 = ::fwComEd::VectorMsg::New();
    msg2->appendRemovedObject(str1);
    msg2->appendRemovedObject(str2);

    CPPUNIT_ASSERT(msg2->hasEvent(::fwComEd::VectorMsg::REMOVED_OBJECTS));

    ::fwData::Vector::sptr vecRemovedObj = msg2->getRemovedObjects();
    CPPUNIT_ASSERT(std::find(vecRemovedObj->begin(), vecRemovedObj->end(), str1) != vecRemovedObj->end());
    CPPUNIT_ASSERT(std::find(vecRemovedObj->begin(), vecRemovedObj->end(), str2) != vecRemovedObj->end());
}

//------------------------------------------------------------------------------

void VectorMsgTest::vectorHelperTest()
{
    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();


    ::fwData::String::sptr str1 = ::fwData::String::New("Str1");
    ::fwData::String::sptr str2 = ::fwData::String::New("Str2");

    ::fwData::Vector::sptr vector = ::fwData::Vector::New();

    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();
    ::fwServices::IService::sptr srv = srvFactory->create("::fwComEd::ut::SVectorTest");
    ::fwServices::OSR::registerService(vector, srv);
    ::fwComEd::ut::SVectorTest::sptr testSrv = ::fwComEd::ut::SVectorTest::dynamicCast(srv);

    srv->start().wait();

    ::fwServices::helper::SigSlotConnection::sptr helper = ::fwServices::helper::SigSlotConnection::New();
    helper->connect( vector, srv, srv->getObjSrvConnections() );

    {
        ::fwComEd::helper::Vector vectorHelper(vector);

        vectorHelper.add(str1);
        CPPUNIT_ASSERT_EQUAL((size_t) 1, vector->size());
        vectorHelper.add(str2);
        CPPUNIT_ASSERT_EQUAL((size_t) 2, vector->size());

        CPPUNIT_ASSERT(std::find(vector->begin(), vector->end(), str1) != vector->end());
        CPPUNIT_ASSERT(std::find(vector->begin(), vector->end(), str2) != vector->end());

        vectorHelper.notify(srv, true);

        // Wait a little notification system
        ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

        CPPUNIT_ASSERT(testSrv->getIsUpdated());

        ::fwData::Vector::sptr addedObjects = testSrv->m_vectMsg->getAddedObjects();

        CPPUNIT_ASSERT(std::find(addedObjects->begin(), addedObjects->end(), str1) != addedObjects->end());
        CPPUNIT_ASSERT(std::find(addedObjects->begin(), addedObjects->end(), str2) != addedObjects->end());
    }

    {
        ::fwComEd::helper::Vector vectorHelper(vector);

        vectorHelper.remove(str1);
        CPPUNIT_ASSERT_EQUAL((size_t) 1, vector->size());
        vectorHelper.remove(str2);
        CPPUNIT_ASSERT_EQUAL((size_t) 0, vector->size());

        vectorHelper.notify(srv, true);

        // Wait a little notification system
        ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

        CPPUNIT_ASSERT(testSrv->getIsUpdated());

        ::fwData::Vector::sptr removedObjects = testSrv->m_vectMsg->getRemovedObjects();

        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), str1) != removedObjects->end());
        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), str2) != removedObjects->end());
    }

    vector->getContainer().push_back(str1);
    vector->getContainer().push_back(str2);

    {
        ::fwComEd::helper::Vector vectorHelper(vector);

        vectorHelper.clear();
        CPPUNIT_ASSERT_EQUAL((size_t) 0, vector->size());

        vectorHelper.notify(srv, true);

        // Wait a little notification system
        ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

        CPPUNIT_ASSERT(testSrv->getIsUpdated());

        ::fwData::Vector::sptr removedObjects = testSrv->m_vectMsg->getRemovedObjects();

        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), str1) != removedObjects->end());
        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), str2) != removedObjects->end());
    }

    srv->stop().wait();;
    ::fwServices::OSR::unregisterService(srv);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwComEd
