/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <core/com/slot.hpp>

#include <core/os/temp_path.hpp>

#include <data/string.hpp>

#include <io/__/service/io_types.hpp>
#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/test/dialog/input.hpp>
#include <ui/test/dialog/location.hpp>
#include <ui/test/dialog/message.hpp>

#include <doctest/doctest.h>

// Returns a basic configuration for reader and writer
inline static sight::service::config_t setup_config(const bool _read = true, const bool _raw = false)
{
    sight::service::config_t config;

    config.put("dialog.<xmlattr>.extension", ".perlimpinpin");
    config.put("dialog.<xmlattr>.description", "Perlimpinpin powder container");
    config.put("dialog.<xmlattr>.policy", "never");

    config.put("password.<xmlattr>.policy", "never");
    config.put("password.<xmlattr>.encryption", "password");

    if(_raw)
    {
        config.put("archive.<xmlattr>.format", "filesystem");
    }
    else if(_read)
    {
        config.put("archive.<xmlattr>.format", "archive");
    }
    else
    {
        config.put("archive.<xmlattr>.format", "optimized");
    }

    return config;
}

//------------------------------------------------------------------------------

inline static void basic_test(const bool _raw = false)
{
    // Create a temporary file
    sight::core::os::temp_dir tmp_dir;
    const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer sight::service
        auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
            sight::service::add("sight::module::io::session::writer")
        );
        CHECK(writer);

        // Set data input
        auto in_string = std::make_shared<sight::data::string>(expected);
        writer->set_input(in_string, sight::io::service::WRITER_DATA_KEY);

        // Set file output
        writer->set_file(tmp_file);

        // Configure the writer sight::service
        writer->set_config(setup_config(false, _raw));
        writer->configure();

        // Execute the writer sight::service
        writer->start().get();
        writer->update().get();
        writer->stop().get();

        // Cleanup
        sight::service::unregister_service(writer);
    }

    // The file should have been created
    CHECK(std::filesystem::exists(tmp_file));
    CHECK(std::filesystem::is_regular_file(tmp_file));
    CHECK(std::filesystem::file_size(tmp_file) > 0);

    {
        // Create a reader sight::service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            sight::service::add("sight::module::io::session::reader")
        );
        CHECK(reader);

        // Set data output
        auto out_string = std::make_shared<sight::data::string>();
        reader->set_inout(out_string, sight::io::service::READER_DATA_KEY);

        // Set file input
        reader->set_file(tmp_file);

        // Create slot connections
        bool session_loaded = false;
        std::filesystem::path session_loaded_path;
        auto session_loaded_slot = sight::core::com::new_slot(
            [&](std::filesystem::path _path)
        {
            session_loaded      = true;
            session_loaded_path = _path;
        });
        session_loaded_slot->set_worker(sight::core::thread::get_default_worker());
        auto conn1 = reader->signal("session_loaded")->connect(session_loaded_slot);

        bool session_loading_failed = false;
        std::filesystem::path session_loading_failed_path;
        auto session_loading_failed_slot = sight::core::com::new_slot(
            [&session_loading_failed, &session_loading_failed_path](std::filesystem::path _path)
        {
            session_loading_failed      = true;
            session_loading_failed_path = _path;
        });
        session_loading_failed_slot->set_worker(sight::core::thread::get_default_worker());
        auto conn2 = reader->signal("session_loading_failed")->connect(session_loading_failed_slot);

        // Configure the reader sight::service
        reader->set_config(setup_config(true, _raw));
        reader->configure();

        // Execute the writer sight::service
        reader->start().get();
        reader->update().get();
        reader->stop().get();

        // Cleanup
        sight::service::unregister_service(reader);

        // Final test
        CHECK(session_loaded);
        CHECK_EQ(session_loaded_path, tmp_file);
        CHECK(!session_loading_failed);
        CHECK_EQ(session_loading_failed_path, std::filesystem::path(""));

        CHECK_EQ(expected, out_string->get_value());
    }
}

//------------------------------------------------------------------------------

static void bad_policy_test(bool _reader, const std::string& _key, const std::string& _value)
{
    auto config = setup_config(_reader);
    config.put(_key, _value);
    sight::service::base::sptr service;
    if(_reader)
    {
        service = sight::service::add("sight::module::io::session::reader");
    }
    else
    {
        service = sight::service::add("sight::module::io::session::writer");
    }

    CHECK(service);
    service->set_config(config);
    CHECK_THROWS_AS(service->configure(), sight::core::exception);
    sight::service::remove(service);
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::module::io::session")
{
    TEST_CASE("basic_archive")
    {
        basic_test(false);
    }

    TEST_CASE("basic_raw")
    {
        basic_test(true);
    }

    TEST_CASE("reader_bad_dialog_policy")
    {
        bad_policy_test(true, "dialog.<xmlattr>.policy", "whenever_i_want");
    }

    TEST_CASE("reader_bad_password_policy")
    {
        bad_policy_test(true, "password.<xmlattr>.policy", "when_i_remember_it");
    }

    TEST_CASE("reader_bad_password_encryption")
    {
        bad_policy_test(true, "password.<xmlattr>.encryption", "sweet");
    }

    TEST_CASE("reader_bad_file")
    {
        // Create a temporary file
        sight::core::os::temp_dir tmp_dir;
        const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

        {
            // Create a reader service
            auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
                sight::service::add("sight::module::io::session::reader")
            );
            CHECK(reader);

            // Set data output
            auto out_string = std::make_shared<sight::data::string>();
            reader->set_inout(out_string, sight::io::service::READER_DATA_KEY);

            // Set file input
            reader->set_file(tmp_file);

            // Create slot connections
            bool session_loaded = false;
            std::filesystem::path session_loaded_path;
            auto session_loaded_slot = sight::core::com::new_slot(
                [&](std::filesystem::path _path)
            {
                session_loaded      = true;
                session_loaded_path = _path;
            });
            session_loaded_slot->set_worker(sight::core::thread::get_default_worker());
            auto conn1 = reader->signal("session_loaded")->connect(session_loaded_slot);

            bool session_loading_failed = false;
            std::filesystem::path session_loading_failed_path;
            auto session_loading_failed_slot = sight::core::com::new_slot(
                [&session_loading_failed, &session_loading_failed_path](std::filesystem::path _path)
            {
                session_loading_failed      = true;
                session_loading_failed_path = _path;
            });
            session_loading_failed_slot->set_worker(sight::core::thread::get_default_worker());
            auto conn2 = reader->signal("session_loading_failed")->connect(session_loading_failed_slot);

            // Configure the reader sight::service
            reader->set_config(setup_config(true, false));
            reader->configure();

            // Execute the writer sight::service
            reader->start().get();
            reader->update().get();
            reader->stop().get();

            // Cleanup
            sight::service::unregister_service(reader);

            // Final test
            CHECK(!session_loaded);
            CHECK_EQ(session_loaded_path, std::filesystem::path(""));
            CHECK(session_loading_failed);
            CHECK_EQ(session_loading_failed_path, tmp_file);
        }
    }

    TEST_CASE("writer_bad_dialog_policy")
    {
        bad_policy_test(false, "dialog.<xmlattr>.policy", "whenever_i_want");
    }

    TEST_CASE("writer_bad_password_policy")
    {
        bad_policy_test(false, "password.<xmlattr>.policy", "when_i_remember_it");
    }

    TEST_CASE("writer_bad_password_encryption")
    {
        bad_policy_test(false, "password.<xmlattr>.encryption", "sweet");
    }

    TEST_CASE("file_dialog")
    {
        // Create a temporary file
        sight::core::os::temp_dir tmp_dir;
        const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

        const std::string expected("Abracadabra");
        {
            // Create a writer sight::service
            auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
                sight::service::add("sight::module::io::session::writer")
            );
            CHECK(writer);

            // Set data input
            auto in_string = std::make_shared<sight::data::string>(expected);
            writer->set_input(in_string, sight::io::service::WRITER_DATA_KEY);

            // Configure the writer sight::service
            auto config = setup_config(false);
            config.put("dialog.<xmlattr>.policy", "always");
            writer->set_config(config);
            writer->configure();

            // Execute the writer sight::service
            writer->start().get();

            sight::ui::test::dialog::location::set_paths({tmp_file});

            writer->update().get();
            writer->stop().get();

            // Cleanup
            sight::service::unregister_service(writer);

            CHECK(sight::ui::test::dialog::location::clear());
        }

        // The file should have been created
        CHECK(std::filesystem::exists(tmp_file));
        CHECK(std::filesystem::is_regular_file(tmp_file));
        CHECK(std::filesystem::file_size(tmp_file) > 0);

        {
            // Create a reader sight::service
            auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
                sight::service::add("sight::module::io::session::reader")
            );
            CHECK(reader);

            // Set data output
            auto out_string = std::make_shared<sight::data::string>();
            reader->set_inout(out_string, sight::io::service::READER_DATA_KEY);

            // Configure the reader sight::service
            auto config = setup_config(true);
            config.put("dialog.<xmlattr>.policy", "always");
            reader->set_config(config);
            reader->configure();

            // Execute the writer sight::service
            reader->start().get();

            sight::ui::test::dialog::location::set_paths({tmp_file});

            reader->update().get();
            reader->stop().get();

            // Cleanup
            sight::service::unregister_service(reader);

            // Final test
            CHECK_EQ(expected, out_string->get_value());

            CHECK(sight::ui::test::dialog::location::clear());
        }
    }

    TEST_CASE("password")
    {
        // Create a temporary file
        sight::core::os::temp_dir tmp_dir;
        const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

        const std::string expected("Abracadabra");
        {
            // Create a writer sight::service
            auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
                sight::service::add("sight::module::io::session::writer")
            );
            CHECK(writer);

            // Set data input
            auto in_string = std::make_shared<sight::data::string>(expected);
            writer->set_input(in_string, sight::io::service::WRITER_DATA_KEY);

            // Set file output
            writer->set_file(tmp_file);

            // Configure the writer sight::service
            auto config = setup_config(false);
            config.put("password.<xmlattr>.policy", "always");
            writer->set_config(config);
            writer->configure();

            // Execute the writer sight::service
            writer->start().get();

            sight::ui::test::dialog::input::push_input("case-sensitive");

            writer->update().get();
            writer->stop().get();

            // Cleanup
            sight::service::unregister_service(writer);

            CHECK(sight::ui::test::dialog::input::clear());
        }

        // The file should have been created
        CHECK(std::filesystem::exists(tmp_file));
        CHECK(std::filesystem::is_regular_file(tmp_file));
        CHECK(std::filesystem::file_size(tmp_file) > 0);

        {
            // Create a reader sight::service
            auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
                sight::service::add("sight::module::io::session::reader")
            );
            CHECK(reader);

            // Set data output
            auto out_string = std::make_shared<sight::data::string>();
            reader->set_inout(out_string, sight::io::service::READER_DATA_KEY);

            // Set file input
            reader->set_file(tmp_file);

            // Configure the reader sight::service
            auto config = setup_config(true);
            config.put("password.<xmlattr>.policy", "always");
            reader->set_config(config);
            reader->configure();

            // Execute the writer sight::service
            reader->start().get();

            sight::ui::test::dialog::input::push_input("Oops");
            sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::retry);
            sight::ui::test::dialog::input::push_input("I forgot");
            sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::retry);
            sight::ui::test::dialog::input::push_input("Wait I remember");
            sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::retry);
            sight::ui::test::dialog::input::push_input("case-sensitive");

            reader->update().get();
            reader->stop().get();

            // Cleanup
            sight::service::unregister_service(reader);

            // Final test
            CHECK_EQ(expected, out_string->get_value());

            CHECK(sight::ui::test::dialog::input::clear());
            CHECK(sight::ui::test::dialog::message::clear());
        }
    }
} // TEST_SUITE
