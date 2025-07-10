/************************************************************************
 *
 * Copyright (C) 2016-2025 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "preferences_test.hpp"

#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <io/zip/archive_reader.hpp>

#include <ui/__/preferences.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::ut::preferences_test);

namespace sight::ui::ut
{

//------------------------------------------------------------------------------

void preferences_test::setUp()
{
    ui::preferences::set_enabled(true);
    ui::preferences::set_password_policy(core::crypto::password_keeper::password_policy::never);

    core::runtime::init();
    core::runtime::load_module("sight::module::ui");

    // Set the profile name
    const std::string& profile_name = core::tools::uuid::generate();
    core::runtime::get_current_profile()->set_name(profile_name);

    // Compute the expected preferences file path
    m_preferences_path = core::tools::os::get_user_config_dir(profile_name) / "preferences.json";
    m_encrypted_path   = core::tools::os::get_user_config_dir(profile_name) / "preferences.sight";

    // Reset the password and disable encryption
    ui::preferences::set_password_policy(core::crypto::password_keeper::password_policy::never);
    ui::preferences::set_encryption_policy(core::crypto::password_keeper::encryption_policy::password);
    ui::preferences::set_password("");
}

//------------------------------------------------------------------------------

void preferences_test::tearDown()
{
    ui::preferences::set_enabled(false);
    std::filesystem::remove_all(core::tools::os::get_user_config_dir(core::runtime::get_current_profile()->name()));
}

//------------------------------------------------------------------------------

void preferences_test::runtime_test()
{
    {
        // Create the preference file
        CPPUNIT_ASSERT_NO_THROW(
            ui::preferences preferences;

            // This mark the preference as modified so it will be saved
            sight::ui::preferences::clear();
        );
    }

    // Check if the preference file really exists
    CPPUNIT_ASSERT(std::filesystem::exists(m_preferences_path) && std::filesystem::is_regular_file(m_preferences_path));
}

//------------------------------------------------------------------------------

void preferences_test::simple_test()
{
    const std::string& root_key     = "ROOT";
    const std::string& string_key   = root_key + ".STRING";
    const std::string& string_value = core::tools::uuid::generate();

    {
        ui::preferences preferences;

        // Check get value from an empty preferences file
        [[maybe_unused]] std::string value;

        CPPUNIT_ASSERT_THROW(
            value = preferences.get<std::string>(root_key),
            boost::property_tree::ptree_error
        );
        CPPUNIT_ASSERT_NO_THROW(value = preferences.get(root_key, string_value));
        CPPUNIT_ASSERT_EQUAL(string_value, value);

        // Check set value
        CPPUNIT_ASSERT_NO_THROW(
            preferences.put(string_key, string_value);
        );
        CPPUNIT_ASSERT_NO_THROW(value = preferences.get<std::string>(string_key));
        const auto& set_value = preferences.get<std::string>(string_key);
        CPPUNIT_ASSERT_EQUAL(string_value, set_value);
    }

    // The preferences file should have been saved
    CPPUNIT_ASSERT(std::filesystem::exists(m_preferences_path) && std::filesystem::is_regular_file(m_preferences_path));
    boost::property_tree::ptree from_disk;
    boost::property_tree::json_parser::read_json(m_preferences_path.string(), from_disk);
    const auto& saved_value = from_disk.get<std::string>(string_key);
    CPPUNIT_ASSERT_EQUAL(string_value, saved_value);
}

//------------------------------------------------------------------------------

void preferences_test::delimeter_test()
{
    const std::string& root_key      = "ROOT";
    const std::string& int_key       = root_key + ".INT";
    const std::string& delimited_key = "%" + int_key + "%";
    const int int_value              = 1664;

    // Set and test an int value
    {
        ui::preferences preferences;

        CPPUNIT_ASSERT_NO_THROW(
            preferences.put(int_key, int_value);
        );
        const auto set_value = preferences.get<int>(int_key);
        CPPUNIT_ASSERT_EQUAL(int_value, set_value);
    }

    // Now test the "delimited" getter
    {
        ui::preferences preferences;

        // Test without delimiter
        const auto no_delimiter_value = preferences.delimited_get(std::to_string(int_value), -1);
        CPPUNIT_ASSERT_EQUAL(int_value, no_delimiter_value);

        // Test with a delimiter
        const auto delimited_value = preferences.delimited_get(delimited_key, -1);
        CPPUNIT_ASSERT_EQUAL(int_value, delimited_value);
    }
}

//------------------------------------------------------------------------------

void preferences_test::parsed_get_test()
{
    const std::string& root_key      = "ROOT";
    const std::string& str_key       = root_key + ".STR";
    const std::string& delimited_key = "%" + str_key + "%";
    const std::string str_value      = "1664";

    ui::preferences preferences;

    CPPUNIT_ASSERT_NO_THROW(
        preferences.put(str_key, str_value);
    );

    // Now test the getter
    {
        // Test without delimiter, will fail as the key won't be found
        // The key will however be returned as the val
        auto [key, val] = preferences.parsed_get<std::string>(str_key);
        CPPUNIT_ASSERT_EQUAL(key, std::string(""));
        CPPUNIT_ASSERT_EQUAL(val, str_key);
    }

    {
        // Test with a delimiter, both values should be returned
        // As the preferneces is valid this time
        auto [key, val] = preferences.parsed_get<std::string>(delimited_key);
        CPPUNIT_ASSERT_EQUAL(key, str_key);
        CPPUNIT_ASSERT_EQUAL(val, str_value);
    }
}

//------------------------------------------------------------------------------

void preferences_test::encrypted_test()
{
    // Setting a password will enable encryption
    ui::preferences::set_password("password");

    const std::string& root_key     = "ROOT";
    const std::string& string_key   = root_key + ".STRING";
    const std::string& string_value = core::tools::uuid::generate();

    {
        ui::preferences preferences;

        [[maybe_unused]] std::string value;

        // Check get value from an empty preferences file
        CPPUNIT_ASSERT_THROW(
            value = preferences.get<std::string>(root_key),
            boost::property_tree::ptree_error
        );
        CPPUNIT_ASSERT_NO_THROW(value = preferences.get(root_key, string_value));
        CPPUNIT_ASSERT_EQUAL(string_value, value);

        // Check set value
        CPPUNIT_ASSERT_NO_THROW(
            preferences.put(string_key, string_value);
        );
        CPPUNIT_ASSERT_NO_THROW(value = preferences.get<std::string>(string_key));
        const auto& set_value = preferences.get<std::string>(string_key);
        CPPUNIT_ASSERT_EQUAL(string_value, set_value);
    }

    {
        // The preferences file should have been saved but as a .sight file
        CPPUNIT_ASSERT(std::filesystem::exists(m_encrypted_path) && std::filesystem::is_regular_file(m_encrypted_path));

        // Open the archive that holds the property tree
        const auto& archive = io::zip::archive_reader::get(m_encrypted_path);

        // Create the input stream, with a password, allowing decoding an encrypted file
        const auto& istream = archive->open_file(
            "preferences.json",
            "password"
        );

        // Read the property tree from the archive
        boost::property_tree::ptree from_disk;
        boost::property_tree::read_json(*istream, from_disk);

        // Test the saved string
        const auto& saved_value = from_disk.get<std::string>(string_key);
        CPPUNIT_ASSERT_EQUAL(string_value, saved_value);
    }

    {
        // This will reset preferences
        ui::preferences::set_enabled(false);
        ui::preferences::set_enabled(true);

        // Set a bad password and see what happens
        ui::preferences::set_password("bad_password!");
        CPPUNIT_ASSERT_THROW(ui::preferences(), ui::bad_password);

        // Setting an empty password will reset to clear json save
        ui::preferences::set_password("");
    }
}

//------------------------------------------------------------------------------

void preferences_test::forced_encryption_test()
{
    // force the encryption, without a password
    ui::preferences::set_encryption_policy(core::crypto::password_keeper::encryption_policy::forced);

    const std::string& root_key     = "ROOT";
    const std::string& string_key   = root_key + ".STRING";
    const std::string& string_value = core::tools::uuid::generate();

    // Write a preference
    {
        ui::preferences preferences;

        // Set value
        CPPUNIT_ASSERT_NO_THROW(
            preferences.put(string_key, string_value);
        );

        // check the value
        [[maybe_unused]] std::string value;
        CPPUNIT_ASSERT_NO_THROW(value = preferences.get<std::string>(string_key));
        CPPUNIT_ASSERT_EQUAL(string_value, value);
    }

    // The preferences file should have been saved but as a .sight file
    CPPUNIT_ASSERT(std::filesystem::exists(m_encrypted_path) && std::filesystem::is_regular_file(m_encrypted_path));

    // This will reset preferences
    ui::preferences::set_enabled(false);
    ui::preferences::set_enabled(true);

    // Read a preference back
    {
        //CPPUNIT_ASSERT_NO_THROW(ui::preferences());
        ui::preferences preferences;

        // check the value
        [[maybe_unused]] std::string value;
        CPPUNIT_ASSERT_NO_THROW(value = preferences.get<std::string>(string_key));
        CPPUNIT_ASSERT_EQUAL(string_value, value);
    }
}

//------------------------------------------------------------------------------

void preferences_test::corrupted_test()
{
    const std::string dummy = "dummy";
    const std::string empty = "empty";

    // Create a dummy preferences file
    {
        ui::preferences preferences;
        sight::ui::preferences::clear();
        preferences.put(dummy, dummy);
    }

    // Reload it
    {
        ui::preferences preferences;
        const auto& value = preferences.get<std::string>(dummy);
        CPPUNIT_ASSERT_EQUAL(dummy, value);
    }

    // Reset the preferences to force a reload and corrupt the file
    {
        // Force a reload
        ui::preferences::set_enabled(false);
        ui::preferences::set_enabled(true);

        std::ofstream ofs(m_preferences_path, std::ios::out | std::ios::trunc);
        CPPUNIT_ASSERT(ofs.is_open());
        ofs << "corrupted content";
        ofs.close();
    }

    // Reload it. It should fail to retrieve the "dummy" key, but should not throw.
    // The default "empty" value should be returned instead.
    try
    {
        ui::preferences preferences;
        const auto& value = preferences.get_optional<std::string>(dummy).value_or(empty);
        CPPUNIT_ASSERT_EQUAL(empty, value);

        preferences.put(dummy, empty);
    }
    catch(...)
    {
        // If we reach this point, it means the preferences were not recreated correctly
        CPPUNIT_FAIL("Preferences should have been loaded with a default value after corruption.");
    }

    // Check that the "empty" value is still there
    {
        // Force a reload
        ui::preferences::set_enabled(false);
        ui::preferences::set_enabled(true);

        ui::preferences preferences;
        const auto& value = preferences.get<std::string>(dummy);
        CPPUNIT_ASSERT_EQUAL(empty, value);
    }
}

} // namespace sight::ui::ut
