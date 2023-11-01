/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "writer_test.hpp"

#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/uuid.hpp>

#include <data/matrix_tl.hpp>

#include <io/__/service/writer.hpp>

#include <service/op.hpp>

#include <ui/__/preferences.hpp>

// cspell:ignore nvjpeg

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::matrix::ut::writer_test);

namespace sight::module::io::matrix::ut
{

static const std::string EXPECTED =
    "1;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;\n"
    "2;2.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;\n"
    "3;3.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;1.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;0.0000000;\n";

static const std::shared_ptr<const sight::data::matrix_tl> SOURCE_TL =
    []
    {
        auto matrix_tl = std::make_shared<sight::data::matrix_tl>();
        matrix_tl->init_pool_size(4);

        const auto fill_tl =
            [&matrix_tl](const core::hires_clock::type _timestamp)
            {
                auto data = matrix_tl->create_buffer(_timestamp);

                std::array<float, 16> matrix {
                    1., 0., 0., 0.,
                    0., 1., 0., 0.,
                    0., 0., 1., 0.,
                    0., 0., 0., 1.
                };

                matrix[0] = float(_timestamp);
                data->set_element(matrix, 0);
                matrix_tl->push_object(data);
            };

        fill_tl(1);
        fill_tl(2);
        fill_tl(3);

        return matrix_tl;
    }();

//------------------------------------------------------------------------------

void writer_test::setUp()
{
    ui::preferences::set_enabled(true);
    ui::preferences::set_password_policy(core::crypto::password_keeper::password_policy::never);

    // Set the profile name
    const std::string& profile_name = core::tools::uuid::generate();
    core::runtime::get_current_profile()->set_name(profile_name);
}

//------------------------------------------------------------------------------

void writer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void writer_test::basic_test()
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Create the service
    auto matrix_writer = service::add("sight::module::io::matrix::matrix_writer");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::io::matrix::matrix_writer'", matrix_writer);
    matrix_writer->set_input(SOURCE_TL, "data");

    // Create the service configuration
    service::config_t config;
    config.add("file", "matrices.csv");

    boost::property_tree::ptree config_child;
    config_child.put("<xmlattr>.interactive", false);
    config.add_child("config", config_child);

    // Start the service
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->set_config(config));
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->configure());
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->start().wait());

    // Test writing in the temporary directory
    matrix_writer->slot("set_base_folder")->run(tmp_dir.string());

    matrix_writer->slot("start_record")->run();
    matrix_writer->slot("write")->run(core::hires_clock::type(1));
    matrix_writer->slot("write")->run(core::hires_clock::type(2));
    matrix_writer->slot("write")->run(core::hires_clock::type(3));
    matrix_writer->slot("stop_record")->run();

    // Stop the service
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->stop().wait());
    service::remove(matrix_writer);

    // Check the result...
    CPPUNIT_ASSERT_EQUAL(
        false,
        std::dynamic_pointer_cast<sight::io::service::writer>(matrix_writer)->has_failed()
    );

    std::ifstream file(tmp_dir / "matrices.csv");
    std::string actual((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    CPPUNIT_ASSERT_EQUAL(EXPECTED, actual);
}

//------------------------------------------------------------------------------

void writer_test::base_folder_test()
{
    // Create a temporary directory
    core::os::temp_dir tmp_dir;

    // Create the service
    auto matrix_writer = service::add("sight::module::io::matrix::matrix_writer");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::io::matrix::matrix_writer'", matrix_writer);
    matrix_writer->set_input(SOURCE_TL, "data");

    // Create the service configuration
    service::config_t config;
    config.add("file", "matrices.csv");

    boost::property_tree::ptree config_child;
    config_child.put("<xmlattr>.interactive", false);
    config.add_child("config", config_child);

    // Start the service
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->set_config(config));
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->configure());
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->start().wait());

    // Start recording immediately (no dialog)
    matrix_writer->slot("start_record")->run();

    // Set the base folder after
    matrix_writer->slot("set_base_folder")->run(tmp_dir.string());

    // Test writing
    matrix_writer->slot("write")->run(core::hires_clock::type(1));
    matrix_writer->slot("write")->run(core::hires_clock::type(2));
    matrix_writer->slot("write")->run(core::hires_clock::type(3));

    // Stop the service (the recording should be also stopped)
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->stop().wait());
    service::remove(matrix_writer);

    // Check the result...
    CPPUNIT_ASSERT_EQUAL(
        false,
        std::dynamic_pointer_cast<sight::io::service::writer>(matrix_writer)->has_failed()
    );

    std::ifstream file(tmp_dir / "matrices.csv");
    std::string actual((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    CPPUNIT_ASSERT_EQUAL(EXPECTED, actual);
}

} // namespace sight::module::io::matrix::ut
