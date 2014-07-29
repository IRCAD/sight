/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>
#include <boost/thread/thread.hpp>
#include <boost/chrono/chrono.hpp>

#include <fwDicomIOExt/dcmtk/helper/Series.hpp>

#include <fwTest/Exception.hpp>

#include <fwThread/Worker.hpp>

#include <fwTools/System.hpp>

#include "SeriesEnquirerTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOExt::ut::SeriesEnquirerTest );

namespace fwDicomIOExt
{
namespace ut
{

static ::fwTest::Exception e(""); // force link with fwTest

//------------------------------------------------------------------------------

void SeriesEnquirerTest::setUp()
{
    m_moveApplicationTitle = "IRCW031";
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::initializeConnection()
{
    // Create the series enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();

    // Initialize the enquirer
    m_seriesEnquirer->initialize("TEST-SCU", "pacs-rd.ircad.fr", 11112, "DCM4CHEE", m_moveApplicationTitle);

    // Connect to the pacs
    m_seriesEnquirer->connect();
    CPPUNIT_ASSERT_MESSAGE("The retriever should be connected to the pacs.",
                m_seriesEnquirer->isConnectedToPacs());

    // Try to send a C-ECHO request to the server
    bool ping = m_seriesEnquirer->pingPacs();
    CPPUNIT_ASSERT_MESSAGE("Unable to ping the pacs.", ping);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();
    CPPUNIT_ASSERT_MESSAGE("The retriever shouldn't be connected to the pacs.",
                !m_seriesEnquirer->isConnectedToPacs());
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullSeriesUsingMoveRetrieveMethod()
{
    // Create the retriever
    m_seriesRetriever = ::fwDicomIOExt::dcmtk::SeriesRetriever::New();
    m_seriesRetriever->initialize(m_moveApplicationTitle, 11110);
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    worker->post(::boost::bind(&::fwDicomIOExt::dcmtk::SeriesRetriever::start, m_seriesRetriever));

    // Create the series enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();
    m_seriesEnquirer->initialize("TEST-SCU", "pacs-rd.ircad.fr", 11112, "DCM4CHEE", m_moveApplicationTitle);
    m_seriesEnquirer->connect();

    // Try to find series on the pacs
    OFList< QRResponse* > responses;
    responses = m_seriesEnquirer->findSeriesByDate("20060101", "20081231");
    ::fwDicomIOExt::dcmtk::helper::Series::releaseResponses(responses);

    // Try to pull series from the pacs
    responses = m_seriesEnquirer->findSeriesByPatientName("BOUABIDA");
    m_seriesEnquirer->pullSeriesUsingMoveRetrieveMethod(
            ::fwDicomIOExt::dcmtk::helper::Series::toSeriesInstanceUIDContainer(responses));
    ::fwDicomIOExt::dcmtk::helper::Series::releaseResponses(responses);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();

    worker.reset();


}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullSeriesUsingGetRetrieveMethod()
{
    // Create the series enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();
    m_seriesEnquirer->initialize("TEST-SCU", "pacs-rd.ircad.fr", 11112, "DCM4CHEE", m_moveApplicationTitle);
    m_seriesEnquirer->connect();

    // Try to pull series from the pacs
    OFList< QRResponse* > responses;
    responses = m_seriesEnquirer->findSeriesByPatientName("BOUABIDA");
    m_seriesEnquirer->pullSeriesUsingGetRetrieveMethod(
            ::fwDicomIOExt::dcmtk::helper::Series::toSeriesInstanceUIDContainer(responses));
    ::fwDicomIOExt::dcmtk::helper::Series::releaseResponses(responses);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();

}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullInstanceUsingMoveRetrieveMethod()
{
    // Create the receiver
    m_seriesRetriever = ::fwDicomIOExt::dcmtk::SeriesRetriever::New();
    m_seriesRetriever->initialize(m_moveApplicationTitle, 11110);
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    worker->post(::boost::bind(&::fwDicomIOExt::dcmtk::SeriesRetriever::start, m_seriesRetriever));

    // Create the series enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();
    m_seriesEnquirer->initialize("TEST-SCU", "pacs-rd.ircad.fr", 11112, "DCM4CHEE", m_moveApplicationTitle);
    m_seriesEnquirer->connect();

    // Try to pull instance from the pacs
    std::string sopInstanceUID = m_seriesEnquirer->findSOPInstanceUID(
            "1.3.46.670589.33.1.1136976332028318543.28411712901005333956", 5);
    CPPUNIT_ASSERT_MESSAGE("Wrong SOP Instance UID.",
            sopInstanceUID == "1.3.46.670589.33.1.16471645312175912258.25955668801046348746");
    m_seriesEnquirer->pullInstanceUsingMoveRetrieveMethod(
            "1.3.46.670589.33.1.1136976332028318543.28411712901005333956", sopInstanceUID);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();

    worker.reset();
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullInstanceUsingGetRetrieveMethod()
{
    // Create the series enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();
    m_seriesEnquirer->initialize("TEST-SCU", "pacs-rd.ircad.fr", 11112, "DCM4CHEE", m_moveApplicationTitle);
    m_seriesEnquirer->connect();

    // Try to pull instance from the pacs
    std::string sopInstanceUID = m_seriesEnquirer->findSOPInstanceUID(
            "1.3.46.670589.33.1.1136976332028318543.28411712901005333956", 5);
    CPPUNIT_ASSERT_MESSAGE("Wrong SOP Instance UID.",
            sopInstanceUID == "1.3.46.670589.33.1.16471645312175912258.25955668801046348746");
    m_seriesEnquirer->pullInstanceUsingGetRetrieveMethod(
            "1.3.46.670589.33.1.1136976332028318543.28411712901005333956", sopInstanceUID);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();
}

} // namespace ut

} // namespace fwDicomIOExt

