/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "SessionTest.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/os/temp_path.hpp>

#include <data/string.hpp>

#include <io/__/service/ioTypes.hpp>
#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/__/dialog/input_dummy.hpp>
#include <ui/__/dialog/location_dummy.hpp>
#include <ui/__/dialog/message_dummy.hpp>
#include <ui/__/macros.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::session::ut::session_test);

SIGHT_REGISTER_GUI(sight::ui::dialog::location_dummy, sight::ui::dialog::location_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::dialog::input_dummy, sight::ui::dialog::input_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::dialog::message_dummy, sight::ui::dialog::message_base::REGISTRY_KEY);

namespace sight::module::io::session::ut
{

//------------------------------------------------------------------------------

void session_test::setUp()
{
}

//------------------------------------------------------------------------------

void session_test::tearDown()
{
}

//------------------------------------------------------------------------------

// Returns a basic configuration for reader and writer
inline static service::config_t setup_config(const bool _read = true, const bool _raw = false)
{
    service::config_t config;

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
    core::os::temp_dir tmp_dir;
    const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
            service::add("sight::module::io::session::writer")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto in_string = std::make_shared<data::string>(expected);
        writer->set_input(in_string, sight::io::service::DATA_KEY);

        // Set file output
        writer->set_file(tmp_file);

        // Configure the writer service
        writer->set_config(setup_config(false, _raw));
        writer->configure();

        // Execute the writer service
        writer->start().wait();
        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregister_service(writer);
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmp_file)
        && std::filesystem::is_regular_file(tmp_file)
        && std::filesystem::file_size(tmp_file) > 0
    );

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::reader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto out_string = std::make_shared<data::string>();
        reader->set_inout(out_string, sight::io::service::DATA_KEY);

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
        auto conn1 = reader->signal("sessionLoaded")->connect(session_loaded_slot);

        bool session_loading_failed = false;
        std::filesystem::path session_loading_failed_path;
        auto session_loading_failed_slot = sight::core::com::new_slot(
            [&session_loading_failed, &session_loading_failed_path](std::filesystem::path _path)
            {
                session_loading_failed      = true;
                session_loading_failed_path = _path;
            });
        session_loading_failed_slot->set_worker(sight::core::thread::get_default_worker());
        auto conn2 = reader->signal("sessionLoadingFailed")->connect(session_loading_failed_slot);

        // Configure the reader service
        reader->set_config(setup_config(true, _raw));
        reader->configure();

        // Execute the writer service
        reader->start().wait();
        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregister_service(reader);

        // Final test
        CPPUNIT_ASSERT(session_loaded);
        CPPUNIT_ASSERT_EQUAL(session_loaded_path, tmp_file);
        CPPUNIT_ASSERT(!session_loading_failed);
        CPPUNIT_ASSERT_EQUAL(session_loading_failed_path, std::filesystem::path(""));

        CPPUNIT_ASSERT_EQUAL(expected, out_string->get_value());
    }
}

//------------------------------------------------------------------------------

void session_test::basic_archive_test()
{
    basic_test(false);
}

//------------------------------------------------------------------------------

void session_test::basic_raw_test()
{
    basic_test(true);
}

//------------------------------------------------------------------------------

static void bad_policy_test(bool _reader, const std::string& _key, const std::string& _value)
{
    auto config = setup_config(_reader);
    config.put(_key, _value);
    service::base::sptr service;
    if(_reader)
    {
        service = service::add("sight::module::io::session::reader");
    }
    else
    {
        service = service::add("sight::module::io::session::writer");
    }

    CPPUNIT_ASSERT(service);
    service->set_config(config);
    CPPUNIT_ASSERT_THROW(service->configure(), sight::core::exception);
}

//------------------------------------------------------------------------------

void session_test::reader_bad_dialog_policy_test()
{
    bad_policy_test(true, "dialog.<xmlattr>.policy", "whenever_i_want");
}

//------------------------------------------------------------------------------

void session_test::reader_bad_password_policy_test()
{
    bad_policy_test(true, "password.<xmlattr>.policy", "when_i_remember_it");
}

//------------------------------------------------------------------------------

void session_test::reader_bad_password_encryption_test()
{
    bad_policy_test(true, "password.<xmlattr>.encryption", "sweet");
}

//------------------------------------------------------------------------------

void session_test::reader_bad_file()
{
    // Create a temporary file
    core::os::temp_dir tmp_dir;
    const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::reader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto out_string = std::make_shared<data::string>();
        reader->set_inout(out_string, sight::io::service::DATA_KEY);

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
        auto conn1 = reader->signal("sessionLoaded")->connect(session_loaded_slot);

        bool session_loading_failed = false;
        std::filesystem::path session_loading_failed_path;
        auto session_loading_failed_slot = sight::core::com::new_slot(
            [&session_loading_failed, &session_loading_failed_path](std::filesystem::path _path)
            {
                session_loading_failed      = true;
                session_loading_failed_path = _path;
            });
        session_loading_failed_slot->set_worker(sight::core::thread::get_default_worker());
        auto conn2 = reader->signal("sessionLoadingFailed")->connect(session_loading_failed_slot);

        // Configure the reader service
        reader->set_config(setup_config(true, false));
        reader->configure();

        // Execute the writer service
        reader->start().wait();
        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregister_service(reader);

        // Final test
        CPPUNIT_ASSERT(!session_loaded);
        CPPUNIT_ASSERT_EQUAL(session_loaded_path, std::filesystem::path(""));
        CPPUNIT_ASSERT(session_loading_failed);
        CPPUNIT_ASSERT_EQUAL(session_loading_failed_path, tmp_file);
    }
}

//------------------------------------------------------------------------------

void session_test::writer_bad_dialog_policy_test()
{
    bad_policy_test(false, "dialog.<xmlattr>.policy", "whenever_i_want");
}

//------------------------------------------------------------------------------

void session_test::writer_bad_password_policy_test()
{
    bad_policy_test(false, "password.<xmlattr>.policy", "when_i_remember_it");
}

//------------------------------------------------------------------------------

void session_test::writer_bad_password_encryption_test()
{
    bad_policy_test(false, "password.<xmlattr>.encryption", "sweet");
}

//------------------------------------------------------------------------------

void session_test::file_dialog_test()
{
    // Create a temporary file
    core::os::temp_dir tmp_dir;
    const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
            service::add("sight::module::io::session::writer")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto in_string = std::make_shared<data::string>(expected);
        writer->set_input(in_string, sight::io::service::DATA_KEY);

        // Configure the writer service
        auto config = setup_config(false);
        config.put("dialog.<xmlattr>.policy", "always");
        writer->set_config(config);
        writer->configure();

        // Execute the writer service
        writer->start().wait();

        sight::ui::dialog::location_dummy::set_paths({tmp_file});

        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregister_service(writer);

        CPPUNIT_ASSERT(sight::ui::dialog::location_dummy::clear());
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmp_file)
        && std::filesystem::is_regular_file(tmp_file)
        && std::filesystem::file_size(tmp_file) > 0
    );

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::reader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto out_string = std::make_shared<data::string>();
        reader->set_inout(out_string, sight::io::service::DATA_KEY);

        // Configure the reader service
        auto config = setup_config(true);
        config.put("dialog.<xmlattr>.policy", "always");
        reader->set_config(config);
        reader->configure();

        // Execute the writer service
        reader->start().wait();

        sight::ui::dialog::location_dummy::set_paths({tmp_file});

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregister_service(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, out_string->get_value());

        CPPUNIT_ASSERT(sight::ui::dialog::location_dummy::clear());
    }
}

//------------------------------------------------------------------------------

void session_test::password_test()
{
    // Create a temporary file
    core::os::temp_dir tmp_dir;
    const auto& tmp_file = tmp_dir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
            service::add("sight::module::io::session::writer")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto in_string = std::make_shared<data::string>(expected);
        writer->set_input(in_string, sight::io::service::DATA_KEY);

        // Set file output
        writer->set_file(tmp_file);

        // Configure the writer service
        auto config = setup_config(false);
        config.put("password.<xmlattr>.policy", "always");
        writer->set_config(config);
        writer->configure();

        // Execute the writer service
        writer->start().wait();

        sight::ui::dialog::input_dummy::push_input("case-sensitive");

        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregister_service(writer);

        CPPUNIT_ASSERT(sight::ui::dialog::input_dummy::clear());
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmp_file)
        && std::filesystem::is_regular_file(tmp_file)
        && std::filesystem::file_size(tmp_file) > 0
    );

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::reader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto out_string = std::make_shared<data::string>();
        reader->set_inout(out_string, sight::io::service::DATA_KEY);

        // Set file input
        reader->set_file(tmp_file);

        // Configure the reader service
        auto config = setup_config(true);
        config.put("password.<xmlattr>.policy", "always");
        reader->set_config(config);
        reader->configure();

        // Execute the writer service
        reader->start().wait();

        sight::ui::dialog::input_dummy::push_input("Oops");
        ui::dialog::message_dummy::push_action(sight::ui::dialog::message_dummy::retry);
        sight::ui::dialog::input_dummy::push_input("I forgot");
        ui::dialog::message_dummy::push_action(sight::ui::dialog::message_dummy::retry);
        sight::ui::dialog::input_dummy::push_input("Wait I remember");
        ui::dialog::message_dummy::push_action(sight::ui::dialog::message_dummy::retry);
        sight::ui::dialog::input_dummy::push_input("case-sensitive");

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregister_service(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, out_string->get_value());

        CPPUNIT_ASSERT(sight::ui::dialog::input_dummy::clear());
        CPPUNIT_ASSERT(ui::dialog::message_dummy::clear());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::session::ut
