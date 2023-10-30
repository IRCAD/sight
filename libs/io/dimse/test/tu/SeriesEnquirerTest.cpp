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

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dimse::ut::series_enquirer_test);

namespace sight::io::dimse::ut
{

//------------------------------------------------------------------------------

void series_enquirer_test::setUp()
{
    // Local application title
    m_local_application_title = "SeriesEnquirerTest";

    // Pacs host name
    char* pacs_host_name = std::getenv("FWTEST_PACS_HOST_NAME");
    CPPUNIT_ASSERT_MESSAGE("The FWTEST_PACS_HOST_NAME environment variable is not set.", pacs_host_name);
    m_pacs_host_name = std::string(pacs_host_name);

    // Pacs application title
    char* pacs_application_title = std::getenv("FWTEST_PACS_APPLICATION_TITLE");
    CPPUNIT_ASSERT_MESSAGE(
        "The FWTEST_PACS_APPLICATION_TITLE environment variable is not set.",
        pacs_application_title
    );
    m_pacs_application_title = std::string(pacs_application_title);

    // Pacs application port (default 11112)
    char* pacs_application_port = std::getenv("FWTEST_PACS_APPLICATION_PORT");
    m_pacs_application_port =
        (pacs_application_port) != nullptr ? (boost::lexical_cast<std::uint16_t>(pacs_application_port)) : 11112;

    // Move application title
    char* move_application_title = std::getenv("FWTEST_MOVE_APPLICATION_TITLE");
    CPPUNIT_ASSERT_MESSAGE(
        "The FWTEST_MOVE_APPLICATION_TITLE environment variable is not set.",
        move_application_title
    );
    m_move_application_title = std::string(move_application_title);

    // Move application port (default 11110)
    char* move_application_port = std::getenv("FWTEST_MOVE_APPLICATION_PORT");
    m_move_application_port =
        (move_application_port) != nullptr ? (boost::lexical_cast<std::uint16_t>(move_application_port)) : 11110;
}

//------------------------------------------------------------------------------

void series_enquirer_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void series_enquirer_test::initialize_connection()
{
    // Create the series enquirer
    m_series_enquirer = std::make_shared<io::dimse::series_enquirer>();

    // Initialize the enquirer
    m_series_enquirer->initialize(
        m_local_application_title,
        m_pacs_host_name,
        m_pacs_application_port,
        m_pacs_application_title,
        m_move_application_title
    );

    // Connect to the pacs
    m_series_enquirer->connect();
    CPPUNIT_ASSERT_MESSAGE(
        "The retriever should be connected to the pacs.",
        m_series_enquirer->is_connected_to_pacs()
    );

    // Try to send a C-ECHO request to the server
    bool ping = m_series_enquirer->ping_pacs();
    CPPUNIT_ASSERT_MESSAGE("Unable to ping the pacs.", ping);

    // Disconnect from the pacs
    m_series_enquirer->disconnect();
    CPPUNIT_ASSERT_MESSAGE(
        "The retriever shouldn't be connected to the pacs.",
        !m_series_enquirer->is_connected_to_pacs()
    );
}

//------------------------------------------------------------------------------

void series_enquirer_test::push_series()
{
    // Create the series enquirer
    m_series_enquirer = std::make_shared<io::dimse::series_enquirer>();
    m_series_enquirer->initialize(
        m_local_application_title,
        m_pacs_host_name,
        m_pacs_application_port,
        m_pacs_application_title,
        m_move_application_title
    );
    m_series_enquirer->connect();

    // Retrieve DICOM instances
    const std::filesystem::path path =
        utest_data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER_FOR_PACS";
    const std::string path_str = boost::algorithm::replace_all_copy(path.string(), "\\", "/");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + path.string() + "' does not exist",
        std::filesystem::exists(path)
    );

    std::vector<std::filesystem::path> filenames;
    io::dicom::helper::dicom_search::search_recursively(path, filenames, false);
    CPPUNIT_ASSERT_EQUAL(std::size_t(129), filenames.size());

    std::vector<std::filesystem::path> paths;
    paths.reserve(filenames.size());
    for(const auto& file : filenames)
    {
        paths.emplace_back(file.string());
    }

    // Try to push instances to the pacs
    m_series_enquirer->push_series(paths);

    // Disconnect from the pacs
    m_series_enquirer->disconnect();
}

//------------------------------------------------------------------------------

void series_enquirer_test::pull_series_using_move_retrieve_method()
{
    // Create the retriever
    m_series_retriever = std::make_shared<io::dimse::series_retriever>();
    m_series_retriever->initialize(m_move_application_title, m_move_application_port);
    core::thread::worker::sptr worker = core::thread::worker::make();
    worker->post([this](auto&& ...){m_series_retriever->start();});

    // Create the series enquirer
    m_series_enquirer = std::make_shared<io::dimse::series_enquirer>();
    m_series_enquirer->initialize(
        m_local_application_title,
        m_pacs_host_name,
        m_pacs_application_port,
        m_pacs_application_title,
        m_move_application_title
    );
    m_series_enquirer->connect();

    // Try to find series on the pacs
    OFList<QRResponse*> responses;
    responses = m_series_enquirer->find_series_by_date("17890101", "17900101");
    io::dimse::helper::series::release_responses(responses);

    // Try to pull series from the pacs
    responses = m_series_enquirer->find_series_by_patient_name("Doe");
    m_series_enquirer->pull_series_using_move_retrieve_method(
        io::dimse::helper::series::to_series_instance_uid_container(responses)
    );
    io::dimse::helper::series::release_responses(responses);

    // Disconnect from the pacs
    m_series_enquirer->disconnect();

    worker.reset();
}

//------------------------------------------------------------------------------

void series_enquirer_test::pull_series_using_get_retrieve_method()
{
    // Create the series enquirer
    m_series_enquirer = std::make_shared<io::dimse::series_enquirer>();
    m_series_enquirer->initialize(
        m_local_application_title,
        m_pacs_host_name,
        m_pacs_application_port,
        m_pacs_application_title,
        m_move_application_title
    );
    m_series_enquirer->connect();

    // Try to pull series from the pacs
    OFList<QRResponse*> responses;
    responses = m_series_enquirer->find_series_by_patient_name("Doe");
    m_series_enquirer->pull_series_using_get_retrieve_method(
        io::dimse::helper::series::to_series_instance_uid_container(responses)
    );
    io::dimse::helper::series::release_responses(responses);

    // Disconnect from the pacs
    m_series_enquirer->disconnect();
}

//------------------------------------------------------------------------------

void series_enquirer_test::pull_instance_using_move_retrieve_method()
{
    // Create the receiver
    m_series_retriever = std::make_shared<io::dimse::series_retriever>();
    m_series_retriever->initialize(m_move_application_title, m_move_application_port);
    core::thread::worker::sptr worker = core::thread::worker::make();
    worker->post([this](auto&& ...){m_series_retriever->start();});

    // Create the series enquirer
    m_series_enquirer = std::make_shared<io::dimse::series_enquirer>();
    m_series_enquirer->initialize(
        m_local_application_title,
        m_pacs_host_name,
        m_pacs_application_port,
        m_pacs_application_title,
        m_move_application_title
    );
    m_series_enquirer->connect();

    // Try to pull instance from the pacs
    std::string sop_instance_uid = m_series_enquirer->find_sop_instance_uid(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        5
    );
    CPPUNIT_ASSERT_MESSAGE(
        "Wrong SOP Instance UID.",
        sop_instance_uid == "1.2.826.0.1.3680043.2.1143.9672927753340405594477112277850697930"
    );
    m_series_enquirer->pull_instance_using_move_retrieve_method(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        sop_instance_uid
    );

    // Disconnect from the pacs
    m_series_enquirer->disconnect();

    worker.reset();
}

//------------------------------------------------------------------------------

void series_enquirer_test::pull_instance_using_get_retrieve_method()
{
    // Create the series enquirer
    m_series_enquirer = std::make_shared<io::dimse::series_enquirer>();
    m_series_enquirer->initialize(
        m_local_application_title,
        m_pacs_host_name,
        m_pacs_application_port,
        m_pacs_application_title,
        m_move_application_title
    );
    m_series_enquirer->connect();

    // Try to pull instance from the pacs
    std::string sop_instance_uid = m_series_enquirer->find_sop_instance_uid(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        5
    );
    CPPUNIT_ASSERT_MESSAGE(
        "Wrong SOP Instance UID.",
        sop_instance_uid == "1.2.826.0.1.3680043.2.1143.9672927753340405594477112277850697930"
    );
    m_series_enquirer->pull_instance_using_get_retrieve_method(
        "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
        sop_instance_uid
    );

    // Disconnect from the pacs
    m_series_enquirer->disconnect();
}

//------------------------------------------------------------------------------

} // namespace sight::io::dimse::ut
