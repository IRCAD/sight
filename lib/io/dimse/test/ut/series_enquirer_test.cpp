/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/thread/worker.hpp>

#include <io/dicom/helper/dicom_search.hpp>
#include <io/dimse/helper/series.hpp>
#include <io/dimse/series_enquirer.hpp>
#include <io/dimse/series_retriever.hpp>

#include <utest/exception.hpp>

#include <utest_data/data.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <dcmtk/config/osconfig.h>

#include <doctest/doctest.h>

namespace sight::io::dimse::ut
{

//------------------------------------------------------------------------------

inline static void setup_variables(
    std::string& _local_application_title,
    std::string& _pacs_host_name,
    std::string& _pacs_application_title,
    std::uint16_t& _pacs_application_port,
    std::string& _move_application_title,
    std::uint16_t& _move_application_port
)
{
    // Local application title
    _local_application_title = "SeriesEnquirerTest";

    // Pacs host name
    char* pacs_host_name = std::getenv("FWTEST_PACS_HOST_NAME");
    CHECK_MESSAGE(pacs_host_name, "The FWTEST_PACS_HOST_NAME environment variable is not set.");
    _pacs_host_name = std::string(pacs_host_name);

    // Pacs application title
    char* pacs_application_title = std::getenv("FWTEST_PACS_APPLICATION_TITLE");
    CHECK_MESSAGE(
        pacs_application_title,
        "The FWTEST_PACS_APPLICATION_TITLE environment variable is not set."
    );
    _pacs_application_title = std::string(pacs_application_title);

    // Pacs application port (default 11112)
    char* pacs_application_port = std::getenv("FWTEST_PACS_APPLICATION_PORT");
    _pacs_application_port =
        (pacs_application_port) != nullptr ? (boost::lexical_cast<std::uint16_t>(pacs_application_port)) : 11112;

    // Move application title
    char* move_application_title = std::getenv("FWTEST_MOVE_APPLICATION_TITLE");
    CHECK_MESSAGE(
        move_application_title,
        "The FWTEST_MOVE_APPLICATION_TITLE environment variable is not set."
    );
    _move_application_title = std::string(move_application_title);

    // Move application port (default 11110)
    char* move_application_port = std::getenv("FWTEST_MOVE_APPLICATION_PORT");
    _move_application_port =
        (move_application_port) != nullptr ? (boost::lexical_cast<std::uint16_t>(move_application_port)) : 11110;
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::io::dimse::series_enquirer" * doctest::skip())
{
    TEST_CASE("initialize_connection")
    {
        std::string local_application_title;
        std::string pacs_host_name;
        std::string pacs_application_title;
        std::uint16_t pacs_application_port = 0;
        std::string move_application_title;
        std::uint16_t move_application_port = 0;

        setup_variables(
            local_application_title,
            pacs_host_name,
            pacs_application_title,
            pacs_application_port,
            move_application_title,
            move_application_port
        );

        // Create the series enquirer
        auto series_enquirer = std::make_shared<io::dimse::series_enquirer>();

        // Initialize the enquirer
        series_enquirer->initialize(
            local_application_title,
            pacs_host_name,
            pacs_application_port,
            pacs_application_title,
            move_application_title
        );

        // Connect to the pacs
        series_enquirer->connect();
        CHECK_MESSAGE(
            series_enquirer->is_connected_to_pacs(),
            "The retriever should be connected to the pacs."
        );

        // Try to send a C-ECHO request to the server
        bool ping = series_enquirer->ping_pacs();
        CHECK_MESSAGE(ping, "Unable to ping the pacs.");

        // Disconnect from the pacs
        series_enquirer->disconnect();
        CHECK_MESSAGE(
            !series_enquirer->is_connected_to_pacs(),
            "The retriever shouldn't be connected to the pacs."
        );
    }

    TEST_CASE("push_series")
    {
        std::string local_application_title;
        std::string pacs_host_name;
        std::string pacs_application_title;
        std::uint16_t pacs_application_port = 0;
        std::string move_application_title;
        std::uint16_t move_application_port = 0;

        setup_variables(
            local_application_title,
            pacs_host_name,
            pacs_application_title,
            pacs_application_port,
            move_application_title,
            move_application_port
        );

        // Create the series enquirer
        auto series_enquirer = std::make_shared<io::dimse::series_enquirer>();
        series_enquirer->initialize(
            local_application_title,
            pacs_host_name,
            pacs_application_port,
            pacs_application_title,
            move_application_title
        );
        series_enquirer->connect();

        // Retrieve DICOM instances
        const std::filesystem::path path =
            utest_data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER_FOR_PACS";

        CHECK(
            std::filesystem::exists(path)
        );

        std::vector<std::filesystem::path> filenames;
        io::dicom::helper::dicom_search::search_recursively(path, filenames, false);
        CHECK_EQ(std::size_t(129), filenames.size());

        std::vector<std::filesystem::path> paths;
        paths.reserve(filenames.size());
        for(const auto& file : filenames)
        {
            paths.emplace_back(file.string());
        }

        // Try to push instances to the pacs
        series_enquirer->push_series(paths);

        // Disconnect from the pacs
        series_enquirer->disconnect();
    }

    TEST_CASE("pull_series_using_move_retrieve_method")
    {
        std::string local_application_title;
        std::string pacs_host_name;
        std::string pacs_application_title;
        std::uint16_t pacs_application_port = 0;
        std::string move_application_title;
        std::uint16_t move_application_port = 0;

        setup_variables(
            local_application_title,
            pacs_host_name,
            pacs_application_title,
            pacs_application_port,
            move_application_title,
            move_application_port
        );

        // Create the retriever
        auto series_retriever = std::make_shared<io::dimse::series_retriever>();
        series_retriever->initialize(move_application_title, move_application_port);
        core::thread::worker::sptr worker = core::thread::worker::make();
        worker->post([&series_retriever](auto&& ...){series_retriever->start();});

        // Create the series enquirer
        auto series_enquirer = std::make_shared<io::dimse::series_enquirer>();
        series_enquirer->initialize(
            local_application_title,
            pacs_host_name,
            pacs_application_port,
            pacs_application_title,
            move_application_title
        );
        series_enquirer->connect();

        // Try to find series on the pacs
        OFList<QRResponse*> responses;
        responses = series_enquirer->find_series_by_date("17890101", "17900101");
        io::dimse::helper::series::release_responses(responses);

        // Try to pull series from the pacs
        responses = series_enquirer->find_series_by_patient_name("Doe");
        series_enquirer->pull_series_using_move_retrieve_method(
            io::dimse::helper::series::to_series_instance_uid_container(responses)
        );
        io::dimse::helper::series::release_responses(responses);

        // Disconnect from the pacs
        series_enquirer->disconnect();

        worker.reset();
    }

    TEST_CASE("pull_series_using_get_retrieve_method")
    {
        std::string local_application_title;
        std::string pacs_host_name;
        std::string pacs_application_title;
        std::uint16_t pacs_application_port = 0;
        std::string move_application_title;
        std::uint16_t move_application_port = 0;

        setup_variables(
            local_application_title,
            pacs_host_name,
            pacs_application_title,
            pacs_application_port,
            move_application_title,
            move_application_port
        );

        // Create the series enquirer
        auto series_enquirer = std::make_shared<io::dimse::series_enquirer>();
        series_enquirer->initialize(
            local_application_title,
            pacs_host_name,
            pacs_application_port,
            pacs_application_title,
            move_application_title
        );
        series_enquirer->connect();

        // Try to pull series from the pacs
        OFList<QRResponse*> responses;
        responses = series_enquirer->find_series_by_patient_name("Doe");
        series_enquirer->pull_series_using_get_retrieve_method(
            io::dimse::helper::series::to_series_instance_uid_container(responses)
        );
        io::dimse::helper::series::release_responses(responses);

        // Disconnect from the pacs
        series_enquirer->disconnect();
    }

    TEST_CASE("pull_instance_using_move_retrieve_method")
    {
        std::string local_application_title;
        std::string pacs_host_name;
        std::string pacs_application_title;
        std::uint16_t pacs_application_port = 0;
        std::string move_application_title;
        std::uint16_t move_application_port = 0;

        setup_variables(
            local_application_title,
            pacs_host_name,
            pacs_application_title,
            pacs_application_port,
            move_application_title,
            move_application_port
        );

        // Create the receiver
        auto series_retriever = std::make_shared<io::dimse::series_retriever>();
        series_retriever->initialize(move_application_title, move_application_port);
        core::thread::worker::sptr worker = core::thread::worker::make();
        worker->post([&series_retriever](auto&& ...){series_retriever->start();});

        // Create the series enquirer
        auto series_enquirer = std::make_shared<io::dimse::series_enquirer>();
        series_enquirer->initialize(
            local_application_title,
            pacs_host_name,
            pacs_application_port,
            pacs_application_title,
            move_application_title
        );
        series_enquirer->connect();

        // Try to pull instance from the pacs
        std::string sop_instance_uid = series_enquirer->find_sop_instance_uid(
            "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
            5
        );
        CHECK_MESSAGE(
            sop_instance_uid == "1.2.826.0.1.3680043.2.1143.9672927753340405594477112277850697930",
            "Wrong SOP Instance UID."
        );
        series_enquirer->pull_instance_using_move_retrieve_method(
            "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
            sop_instance_uid
        );

        // Disconnect from the pacs
        series_enquirer->disconnect();

        worker.reset();
    }

    TEST_CASE("pull_instance_using_get_retrieve_method")
    {
        std::string local_application_title;
        std::string pacs_host_name;
        std::string pacs_application_title;
        std::uint16_t pacs_application_port = 0;
        std::string move_application_title;
        std::uint16_t move_application_port = 0;

        setup_variables(
            local_application_title,
            pacs_host_name,
            pacs_application_title,
            pacs_application_port,
            move_application_title,
            move_application_port
        );

        // Create the series enquirer
        auto series_enquirer = std::make_shared<io::dimse::series_enquirer>();
        series_enquirer->initialize(
            local_application_title,
            pacs_host_name,
            pacs_application_port,
            pacs_application_title,
            move_application_title
        );
        series_enquirer->connect();

        // Try to pull instance from the pacs
        std::string sop_instance_uid = series_enquirer->find_sop_instance_uid(
            "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
            5
        );
        CHECK_MESSAGE(
            sop_instance_uid == "1.2.826.0.1.3680043.2.1143.9672927753340405594477112277850697930",
            "Wrong SOP Instance UID."
        );
        series_enquirer->pull_instance_using_get_retrieve_method(
            "1.2.826.0.1.3680043.2.1143.6877747397629659013303967301941157856",
            sop_instance_uid
        );

        // Disconnect from the pacs
        series_enquirer->disconnect();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::dimse::ut
