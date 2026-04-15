/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <doctest/doctest.h>

// cspell:ignore nvjpeg

namespace
{

struct fixture
{
    fixture()
    {
        sight::ui::preferences::set_enabled(true);
        sight::ui::preferences::set_password_policy(sight::core::crypto::password_keeper::password_policy::never);

        // Set the profile name
        const std::string& profile_name = sight::core::tools::uuid::generate();
        sight::core::runtime::get_current_profile()->set_name(profile_name);
    }
};

} // namespace

TEST_SUITE("sight::module::io::matrix::writer")
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
                [&matrix_tl](const sight::core::clock::type _timestamp)
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

    TEST_CASE_FIXTURE(fixture, "basic")
    {
        // Create a temporary directory
        sight::core::os::temp_dir tmp_dir;

        // Create the service
        auto matrix_writer = sight::service::add("sight::module::io::matrix::matrix_writer");
        CHECK(matrix_writer);
        matrix_writer->set_input(SOURCE_TL, "data");

        // Create the service configuration
        sight::service::config_t config;
        config.add("file", "matrices.csv");

        boost::property_tree::ptree config_child;
        config_child.put("<xmlattr>.interactive", false);
        config.add_child("config", config_child);

        // Start the service
        CHECK_NOTHROW(matrix_writer->set_config(config));
        CHECK_NOTHROW(matrix_writer->configure());
        CHECK_NOTHROW(matrix_writer->start().wait());

        // Test writing in the temporary directory
        matrix_writer->slot("set_base_folder")->run(tmp_dir.string());

        matrix_writer->slot("start_record")->run();
        matrix_writer->slot("write")->run(sight::core::clock::type(1));
        matrix_writer->slot("write")->run(sight::core::clock::type(2));
        matrix_writer->slot("write")->run(sight::core::clock::type(3));
        matrix_writer->slot("stop_record")->run();

        // Stop the service
        CHECK_NOTHROW(matrix_writer->stop().wait());
        sight::service::remove(matrix_writer);

        // Check the result...
        CHECK_EQ(
            false,
            std::dynamic_pointer_cast<sight::io::service::writer>(matrix_writer)->has_failed()
        );

        std::ifstream file(tmp_dir / "matrices.csv");
        std::string actual((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        CHECK_EQ(EXPECTED, actual);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "base_folder")
    {
        // Create a temporary directory
        sight::core::os::temp_dir tmp_dir;

        // Create the service
        auto matrix_writer = sight::service::add("sight::module::io::matrix::matrix_writer");
        CHECK(matrix_writer);
        matrix_writer->set_input(SOURCE_TL, "data");

        // Create the service configuration
        sight::service::config_t config;
        config.add("file", "matrices.csv");

        boost::property_tree::ptree config_child;
        config_child.put("<xmlattr>.interactive", false);
        config.add_child("config", config_child);

        // Start the service
        CHECK_NOTHROW(matrix_writer->set_config(config));
        CHECK_NOTHROW(matrix_writer->configure());
        CHECK_NOTHROW(matrix_writer->start().wait());

        // Start recording immediately (no dialog)
        matrix_writer->slot("start_record")->run();

        // Set the base folder after
        matrix_writer->slot("set_base_folder")->run(tmp_dir.string());

        // Test writing
        matrix_writer->slot("write")->run(sight::core::clock::type(1));
        matrix_writer->slot("write")->run(sight::core::clock::type(2));
        matrix_writer->slot("write")->run(sight::core::clock::type(3));

        // Stop the service (the recording should be also stopped)
        CHECK_NOTHROW(matrix_writer->stop().wait());
        sight::service::remove(matrix_writer);

        // Check the result...
        CHECK_EQ(
            false,
            std::dynamic_pointer_cast<sight::io::service::writer>(matrix_writer)->has_failed()
        );

        std::ifstream file(tmp_dir / "matrices.csv");
        std::string actual((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        CHECK_EQ(EXPECTED, actual);
    }
} // TEST_SUITE
