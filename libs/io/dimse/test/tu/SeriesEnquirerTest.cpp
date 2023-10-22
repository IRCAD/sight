/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "SeriesEnquirerTest.hpp"

#include "io/dicom/helper/DicomSearch.hpp"

#include <core/thread/worker.hpp>

#include <io/dimse/helper/series.hpp>

#include <utest/exception.hpp>

#include <utest_data/Data.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <dcmtk/config/osconfig.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dimse::ut::SeriesEnquirerTest);

namespace sight::io::dimse::ut
{

//------------------------------------------------------------------------------

void SeriesEnquirerTest::setUp()
{
    // Local application title
    m_localApplicationTitle = "SeriesEnquirerTest";

    // Pacs host name
    char* pacs_host_name = std::getenv("FWTEST_PACS_HOST_NAME");
    CPPUNIT_ASSERT_MESSAGE("The FWTEST_PACS_HOST_NAME environment variable is not set.", pacs_host_name);
    m_pacsHostName = std::string(pacs_host_name);

    // Pacs application title
    char* pacs_application_title = std::getenv("FWTEST_PACS_APPLICATION_TITLE");
    CPPUNIT_ASSERT_MESSAGE(
        "The FWTEST_PACS_APPLICATION_TITLE environment variable is not set.",
        pacs_application_title
    );
    m_pacsApplicationTitle = std::string(pacs_application_title);

    // Pacs application port (default 11112)
    char* pacs_application_port = std::getenv("FWTEST_PACS_APPLICATION_PORT");
    m_pacsApplicationPort =
        (pacs_application_port) != nullptr ? (boost::lexical_cast<std::uint16_t>(pacs_application_port)) : 11112;

    // Move application title
    char* move_application_title = std::getenv("FWTEST_MOVE_APPLICATION_TITLE");
    CPPUNIT_ASSERT_MESSAGE(
        "The FWTEST_MOVE_APPLICATION_TITLE environment variable is not set.",
        move_application_title
    );
    m_moveApplicationTitle = std::string(move_application_title);

    // Move application port (default 11110)
    char* move_application_port = std::getenv("FWTEST_MOVE_APPLICATION_PORT");
    m_moveApplicationPort =
        (move_application_port) != nullptr ? (boost::lexical_cast<std::uint16_t>(move_application_port)) : 11110;
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
    m_seriesEnquirer = std::make_shared<io::dimse::SeriesEnquirer>();

    // Initialize the enquirer
    m_seriesEnquirer->initialize(
        m_localApplicationTitle,
        m_pacsHostName,
        m_pacsApplicationPort,
        m_pacsApplicationTitle,
        m_moveApplicationTitle
    );

    // Connect to the pacs
    m_seriesEnquirer->connect();
    CPPUNIT_ASSERT_MESSAGE(
        "The retriever should be connected to the pacs.",
        m_seriesEnquirer->isConnectedToPacs()
    );

    // Try to send a C-ECHO request to the server
    bool ping = m_seriesEnquirer->pingPacs();
    CPPUNIT_ASSERT_MESSAGE("Unable to ping the pacs.", ping);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();
    CPPUNIT_ASSERT_MESSAGE(
        "The retriever shouldn't be connected to the pacs.",
        !m_seriesEnquirer->isConnectedToPacs()
    );
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pushSeries()
{
    // Create the series enquirer
    m_seriesEnquirer = std::make_shared<io::dimse::SeriesEnquirer>();
    m_seriesEnquirer->initialize(
        m_localApplicationTitle,
        m_pacsHostName,
        m_pacsApplicationPort,
        m_pacsApplicationTitle,
        m_moveApplicationTitle
    );
    m_seriesEnquirer->connect();

    // Retrieve DICOM instances
    const std::filesystem::path path =
        utest_data::Data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER_FOR_PACS";
    const std::string path_str = boost::algorithm::replace_all_copy(path.string(), "\\", "/");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    std::vector<std::filesystem::path> filenames;
    io::dicom::helper::DicomSearch::searchRecursively(path, filenames, false);
    CPPUNIT_ASSERT_EQUAL(std::size_t(129), filenames.size());

    std::vector<std::filesystem::path> paths;
    paths.reserve(filenames.size());
    for(const auto& file : filenames)
    {
        paths.emplace_back(file.string());
    }

    // Try to push instances to the pacs
    m_seriesEnquirer->pushSeries(paths);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullSeriesUsingMoveRetrieveMethod()
{
    // Create the retriever
    m_seriesRetriever = std::make_shared<io::dimse::SeriesRetriever>();
    m_seriesRetriever->initialize(m_moveApplicationTitle, m_moveApplicationPort);
    core::thread::worker::sptr worker = core::thread::worker::make();
    worker->post([this](auto&& ...){m_seriesRetriever->start();});

    // Create the series enquirer
    m_seriesEnquirer = std::make_shared<io::dimse::SeriesEnquirer>();
    m_seriesEnquirer->initialize(
        m_localApplicationTitle,
        m_pacsHostName,
        m_pacsApplicationPort,
        m_pacsApplicationTitle,
        m_moveApplicationTitle
    );
    m_seriesEnquirer->connect();

    // Try to find series on the pacs
    OFList<QRResponse*> responses;
    responses = m_seriesEnquirer->findSeriesByDate("17890101", "17900101");
    io::dimse::helper::series::releaseResponses(responses);

    // Try to pull series from the pacs
    responses = m_seriesEnquirer->findSeriesByPatientName("Doe");
    m_seriesEnquirer->pullSeriesUsingMoveRetrieveMethod(
        io::dimse::helper::series::toSeriesInstanceUIDContainer(responses)
    );
    io::dimse::helper::series::releaseResponses(responses);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();

    worker.reset();
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullSeriesUsingGetRetrieveMethod()
{
    // Create the series enquirer
    m_seriesEnquirer = std::make_shared<io::dimse::SeriesEnquirer>();
    m_seriesEnquirer->initialize(
        m_localApplicationTitle,
        m_pacsHostName,
        m_pacsApplicationPort,
        m_pacsApplicationTitle,
        m_moveApplicationTitle
    );
    m_seriesEnquirer->connect();

    // Try to pull series from the pacs
    OFList<QRResponse*> responses;
    responses = m_seriesEnquirer->findSeriesByPatientName("Doe");
    m_seriesEnquirer->pullSeriesUsingGetRetrieveMethod(
        io::dimse::helper::series::toSeriesInstanceUIDContainer(responses)
    );
    io::dimse::helper::series::releaseResponses(responses);

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullInstanceUsingMoveRetrieveMethod()
{
    // Create the receiver
    m_seriesRetriever = std::make_shared<io::dimse::SeriesRetriever>();
    m_seriesRetriever->initialize(m_moveApplicationTitle, m_moveApplicationPort);
    core::thread::worker::sptr worker = core::thread::worker::make();
    worker->post([this](auto&& ...){m_seriesRetriever->start();});

    // Create the series enquirer
    m_seriesEnquirer = std::make_shared<io::dimse::SeriesEnquirer>();
    m_seriesEnquirer->initialize(
        m_localApplicationTitle,
        m_pacsHostName,
        m_pacsApplicationPort,
        m_pacsApplicationTitle,
        m_moveApplicationTitle
    );
    m_seriesEnquirer->connect();

    // Try to pull instance from the pacs
    std::string sop_instance_uid = m_seriesEnquirer->findSOPInstanceUID(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        5
    );
    CPPUNIT_ASSERT_MESSAGE(
        "Wrong SOP Instance UID.",
        sop_instance_uid == "1.2.826.0.1.3680043.2.1143.9672927753340405594477112277850697930"
    );
    m_seriesEnquirer->pullInstanceUsingMoveRetrieveMethod(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        sop_instance_uid
    );

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();

    worker.reset();
}

//------------------------------------------------------------------------------

void SeriesEnquirerTest::pullInstanceUsingGetRetrieveMethod()
{
    // Create the series enquirer
    m_seriesEnquirer = std::make_shared<io::dimse::SeriesEnquirer>();
    m_seriesEnquirer->initialize(
        m_localApplicationTitle,
        m_pacsHostName,
        m_pacsApplicationPort,
        m_pacsApplicationTitle,
        m_moveApplicationTitle
    );
    m_seriesEnquirer->connect();

    // Try to pull instance from the pacs
    std::string sop_instance_uid = m_seriesEnquirer->findSOPInstanceUID(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        5
    );
    CPPUNIT_ASSERT_MESSAGE(
        "Wrong SOP Instance UID.",
        sop_instance_uid == "1.2.826.0.1.3680043.2.1143.9672927753340405594477112277850697930"
    );
    m_seriesEnquirer->pullInstanceUsingGetRetrieveMethod(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        sop_instance_uid
    );

    // Disconnect from the pacs
    m_seriesEnquirer->disconnect();
}

//------------------------------------------------------------------------------

} // namespace sight::io::dimse::ut
