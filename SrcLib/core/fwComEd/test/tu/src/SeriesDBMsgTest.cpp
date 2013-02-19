/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/Series.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwComEd/helper/SeriesDB.hpp>

#include "SeriesDBMsgTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwComEd::ut::SeriesDBMsgTest );


fwServicesRegisterMacro( ::fwComEd::ut::TestService , ::fwComEd::ut::SSeriesDBTest , ::fwMedData::SeriesDB ) ;

namespace fwComEd
{
namespace ut
{

//------------------------------------------------------------------------------

void SeriesDBMsgTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SeriesDBMsgTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesDBMsgTest::basicTest()
{
    ::fwMedData::ImageSeries::sptr series1 = ::fwMedData::ImageSeries::New();
    ::fwMedData::ModelSeries::sptr series2 = ::fwMedData::ModelSeries::New();

    ::fwComEd::SeriesDBMsg::sptr msg = ::fwComEd::SeriesDBMsg::New();
    msg->appendAddedSeries(series1);
    msg->appendAddedSeries(series2);

    CPPUNIT_ASSERT(msg->hasEvent(::fwComEd::SeriesDBMsg::ADDED_OBJECTS));

    ::fwData::Vector::sptr vecAddedObj = msg->getAddedSeries();
    CPPUNIT_ASSERT(std::find(vecAddedObj->begin(), vecAddedObj->end(), series1) != vecAddedObj->end());
    CPPUNIT_ASSERT(std::find(vecAddedObj->begin(), vecAddedObj->end(), series2) != vecAddedObj->end());

    ::fwComEd::SeriesDBMsg::sptr msg2 = ::fwComEd::SeriesDBMsg::New();
    msg2->appendRemovedSeries(series1);
    msg2->appendRemovedSeries(series2);

    CPPUNIT_ASSERT(msg2->hasEvent(::fwComEd::SeriesDBMsg::REMOVED_OBJECTS));

    ::fwData::Vector::sptr vecRemovedObj = msg2->getRemovedSeries();
    CPPUNIT_ASSERT(std::find(vecRemovedObj->begin(), vecRemovedObj->end(), series1) != vecRemovedObj->end());
    CPPUNIT_ASSERT(std::find(vecRemovedObj->begin(), vecRemovedObj->end(), series2) != vecRemovedObj->end());
}

//------------------------------------------------------------------------------

void SeriesDBMsgTest::seriesDBHelperTest()
{
    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    ::fwMedData::ImageSeries::sptr series1 = ::fwMedData::ImageSeries::New();
    ::fwMedData::ModelSeries::sptr series2 = ::fwMedData::ModelSeries::New();

    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();
    ::fwServices::IService::sptr srv = srvFactory->create("::fwComEd::ut::SSeriesDBTest");
    ::fwServices::OSR::registerService(seriesDB, srv);
    ::fwComEd::ut::SSeriesDBTest::sptr testSrv = ::fwComEd::ut::SSeriesDBTest::dynamicCast(srv);

    srv->start().wait();

    ::fwServices::helper::SigSlotConnection::sptr helper = ::fwServices::helper::SigSlotConnection::New();
    helper->connect( seriesDB, srv, srv->getObjSrvConnections() );

    // Test add series
    {
        ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);

        seriesDBHelper.add(series1);
        CPPUNIT_ASSERT_EQUAL((size_t) 1, seriesDB->size());
        seriesDBHelper.add(series2);
        CPPUNIT_ASSERT_EQUAL((size_t) 2, seriesDB->size());

        CPPUNIT_ASSERT(std::find(seriesDB->begin(), seriesDB->end(), series1) != seriesDB->end());
        CPPUNIT_ASSERT(std::find(seriesDB->begin(), seriesDB->end(), series2) != seriesDB->end());

        seriesDBHelper.notify(srv);

        // Wait a little notification system
        ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

        CPPUNIT_ASSERT(testSrv->getIsUpdated());

        ::fwData::Vector::sptr addedObjects = testSrv->m_seriesDBMsg->getAddedSeries();

        CPPUNIT_ASSERT(std::find(addedObjects->begin(), addedObjects->end(), series1) != addedObjects->end());
        CPPUNIT_ASSERT(std::find(addedObjects->begin(), addedObjects->end(), series2) != addedObjects->end());
    }

    // Test remove series
    {
        ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);

        seriesDBHelper.remove(series1);
        CPPUNIT_ASSERT_EQUAL((size_t) 1, seriesDB->size());
        seriesDBHelper.remove(series2);
        CPPUNIT_ASSERT_EQUAL((size_t) 0, seriesDB->size());

        seriesDBHelper.notify(srv);

        // Wait a little notification system
        ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

        CPPUNIT_ASSERT(testSrv->getIsUpdated());

        ::fwData::Vector::sptr removedObjects = testSrv->m_seriesDBMsg->getRemovedSeries();

        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), series1) != removedObjects->end());
        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), series2) != removedObjects->end());
    }

    seriesDB->getContainer().push_back(series1);
    seriesDB->getContainer().push_back(series2);

    // Test clear series
    {
        ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);

        seriesDBHelper.clear();
        CPPUNIT_ASSERT_EQUAL((size_t) 0, seriesDB->size());

        seriesDBHelper.notify(srv);

        // Wait a little notification system
        ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

        CPPUNIT_ASSERT(testSrv->getIsUpdated());

        ::fwData::Vector::sptr removedObjects = testSrv->m_seriesDBMsg->getRemovedSeries();

        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), series1) != removedObjects->end());
        CPPUNIT_ASSERT(std::find(removedObjects->begin(), removedObjects->end(), series2) != removedObjects->end());
    }

    seriesDB->getContainer().push_back(series1);
    seriesDB->getContainer().push_back(series2);

    // Test merge SeriesDB
    {
        ::fwMedData::ImageSeries::sptr series3 = ::fwMedData::ImageSeries::New();
        ::fwMedData::SeriesDB::sptr localSeriesDB = ::fwMedData::SeriesDB::New();
        localSeriesDB->getContainer().push_back(series3);

        ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);
        seriesDBHelper.merge(localSeriesDB);

        CPPUNIT_ASSERT_EQUAL((size_t) 3, seriesDB->size());

        seriesDBHelper.notify(srv);

        // Wait a little notification system
        ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

        CPPUNIT_ASSERT(testSrv->getIsUpdated());

        ::fwData::Vector::sptr addedObjects = testSrv->m_seriesDBMsg->getAddedSeries();

        CPPUNIT_ASSERT(std::find(addedObjects->begin(), addedObjects->end(), series3) != addedObjects->end());
    }

    srv->stop().wait();
    ::fwServices::OSR::unregisterService(srv);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwComEd
