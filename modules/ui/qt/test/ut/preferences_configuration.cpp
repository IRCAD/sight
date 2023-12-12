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

#include "preferences_configuration.hpp"

#include "loader.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/parameter.hpp>
#include <ui/__/preferences.hpp>
#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <QAbstractButton>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTimer>
#include <QWindow>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::preferences_configuration);

namespace sight::module::ui::qt::ut
{

preferences_configuration::~preferences_configuration()
{
    // Remove the preferences file(s)
    if(const auto& root = m_preferences_path.parent_path(); std::filesystem::exists(root))
    {
        std::filesystem::remove_all(m_preferences_path.parent_path());
    }
}

//------------------------------------------------------------------------------

void preferences_configuration::setUp()
{
    // Set up context before running a test.
    CPPUNIT_ASSERT(qApp != nullptr);

    const std::string& profile_name = sight::core::tools::uuid::generate();
    sight::core::runtime::get_current_profile()->set_name(profile_name);

    sight::ui::preferences::set_enabled(true);
    sight::ui::preferences::set_password_policy(core::crypto::password_keeper::password_policy::never);
    m_preferences_path = core::tools::os::get_user_config_dir(profile_name) / "preferences.json";

    // Clear the preferences file
    sight::ui::preferences preferences;
    sight::ui::preferences::clear();

    // Build container
    std::tie(m_container, m_child_uuid) = make_container();
}

//------------------------------------------------------------------------------

void preferences_configuration::tearDown()
{
    // Destroy container
    destroy_container(m_container);

    // Reset preferences context after running a test.
    sight::ui::preferences::set_enabled(false);

    // Remove the preferences file(s)
    if(const auto& root = m_preferences_path.parent_path(); std::filesystem::exists(root))
    {
        std::filesystem::remove_all(m_preferences_path.parent_path());
    }

    m_preferences_path.clear();
}

//------------------------------------------------------------------------------

void preferences_configuration::widgets_test()
{
    // Register the service
    sight::service::base::sptr preferences_configuration(
        service::add("sight::module::ui::qt::preferences_configuration", m_child_uuid)
    );

    // Will stop the service and unregister it when destroyed
    service_cleaner cleaner(preferences_configuration);

    // Build preferences_configuration configuration
    service::config_t config;

    service::config_t text_preference;
    text_preference.put("type", "text");
    text_preference.put("name", "text");
    text_preference.put("key", "text");
    text_preference.put("default_value", "text");
    config.add_child("preference", text_preference);

    service::config_t path_preference;
    path_preference.put("type", "path");
    path_preference.put("name", "path");
    path_preference.put("key", "path");
    path_preference.put("default_value", "/highway/to/hell");
    config.add_child("preference", path_preference);

    service::config_t file_preference;
    file_preference.put("type", "file");
    file_preference.put("name", "file");
    file_preference.put("key", "file");
    file_preference.put("default_value", "/stairway/to/heaven");
    config.add_child("preference", file_preference);

    service::config_t checkbox_preference;
    checkbox_preference.put("type", "checkbox");
    checkbox_preference.put("name", "checkbox");
    checkbox_preference.put("key", "checkbox");
    checkbox_preference.put("default_value", "true");
    config.add_child("preference", checkbox_preference);

    service::config_t int_preference;
    int_preference.put("type", "int");
    int_preference.put("name", "int");
    int_preference.put("key", "int");
    int_preference.put("min", "0");
    int_preference.put("max", "1000");
    int_preference.put("default_value", "666");
    config.add_child("preference", int_preference);

    service::config_t double_preference;
    double_preference.put("type", "double");
    double_preference.put("name", "double");
    double_preference.put("key", "double");
    double_preference.put("min", "0.0");
    double_preference.put("max", "1000.0");
    double_preference.put("default_value", "666.666");
    config.add_child("preference", double_preference);

    service::config_t list_preference;
    list_preference.put("type", "list");
    list_preference.put("name", "list");
    list_preference.put("key", "list");
    list_preference.put("separator", ";");
    list_preference.put("default_value", "1;2;3");
    config.add_child("preference", list_preference);

    service::config_t combobox_preference;
    combobox_preference.put("type", "combobox");
    combobox_preference.put("name", "combobox");
    combobox_preference.put("key", "combobox");
    combobox_preference.put("separator", ";");
    combobox_preference.put("values", "0.3;2000;banana");
    combobox_preference.put("default_value", "banana");
    config.add_child("preference", combobox_preference);

    // Configure and start the service
    CPPUNIT_ASSERT_NO_THROW(preferences_configuration->configure(config));
    CPPUNIT_ASSERT_NO_THROW(preferences_configuration->start().get());

    bool text_done     = false;
    bool path_done     = false;
    bool file_done     = false;
    bool checkbox_done = false;
    bool int_done      = false;
    bool double_done   = false;
    bool list_done     = false;
    bool combobox_done = false;

    auto script = wait_for_widget(
        [&](QWidget* _widget)
        {
            if(_widget != nullptr && _widget->objectName().startsWith("preferences_configuration"))
            {
                if(!text_done)
                {
                    if(auto* text = _widget->findChild<QLineEdit*>("text"); text != nullptr)
                    {
                        QPointer<QLineEdit> text_ptr(text);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [text_ptr]
                        {
                            if(!text_ptr.isNull())
                            {
                                text_ptr->setText("modified");
                            }
                        }).get();

                        text_done = true;
                    }
                }

                if(!path_done)
                {
                    if(auto* path = _widget->findChild<QLineEdit*>("path"); path != nullptr)
                    {
                        QPointer<QLineEdit> path_ptr(path);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [path_ptr]
                        {
                            if(!path_ptr.isNull())
                            {
                                path_ptr->setText("modified");
                            }
                        }).get();

                        path_done = true;
                    }
                }

                if(!file_done)
                {
                    if(auto* file = _widget->findChild<QLineEdit*>("file"); file != nullptr)
                    {
                        QPointer<QLineEdit> file_ptr(file);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [file_ptr]
                        {
                            if(!file_ptr.isNull())
                            {
                                file_ptr->setText("modified");
                            }
                        }).get();

                        file_done = true;
                    }
                }

                if(!checkbox_done)
                {
                    if(auto* checkbox = _widget->findChild<QCheckBox*>("checkbox"); checkbox != nullptr)
                    {
                        QPointer<QCheckBox> checkbox_ptr(checkbox);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [checkbox_ptr]
                        {
                            if(!checkbox_ptr.isNull())
                            {
                                checkbox_ptr->setChecked(false);
                            }
                        }).get();

                        checkbox_done = true;
                    }
                }

                if(!int_done)
                {
                    if(auto* int_widget = _widget->findChild<QLineEdit*>("int"); int_widget != nullptr)
                    {
                        QPointer<QLineEdit> int_ptr(int_widget);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [int_ptr]
                        {
                            if(!int_ptr.isNull())
                            {
                                int_ptr->setText("42");
                            }
                        }).get();

                        int_done = true;
                    }
                }

                if(!double_done)
                {
                    if(auto* double_widget = _widget->findChild<QLineEdit*>("double"); double_widget != nullptr)
                    {
                        QPointer<QLineEdit> double_ptr(double_widget);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [double_ptr]
                        {
                            if(!double_ptr.isNull())
                            {
                                double_ptr->setText("42.0");
                            }
                        }).get();

                        double_done = true;
                    }
                }

                if(!list_done)
                {
                    if(auto* list = _widget->findChild<QLineEdit*>("list"); list != nullptr)
                    {
                        QPointer<QLineEdit> list_ptr(list);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [list_ptr]
                        {
                            if(!list_ptr.isNull())
                            {
                                list_ptr->setText("3;2;1");
                            }
                        }).get();

                        list_done = true;
                    }
                }

                if(!combobox_done)
                {
                    if(auto* combobox = _widget->findChild<QComboBox*>("combobox"); combobox != nullptr)
                    {
                        QPointer<QComboBox> combobox_ptr(combobox);

                        // Send back to the main thread
                        core::thread::get_default_worker()->post_task<void>(
                            [combobox_ptr]
                        {
                            if(!combobox_ptr.isNull())
                            {
                                combobox_ptr->setCurrentIndex(2);
                            }
                        }).get();

                        combobox_done = true;
                    }
                }

                // Schedule the click on the OK button
                if(text_done
                   && path_done
                   && file_done
                   && checkbox_done
                   && int_done
                   && double_done
                   && list_done
                   && combobox_done)
                {
                    const bool clicked = click_on_button("OK", "preferences_configuration").get();
                    CPPUNIT_ASSERT_MESSAGE("The OK button should have been clicked", clicked);
                    return clicked;
                }
            }

            return false;
        });

    // Update the service: will show the dialog with an OK button
    CPPUNIT_ASSERT_EQUAL(
        std::future_status::ready,
        preferences_configuration->update().wait_for(std::chrono::seconds(10))
    );

    // Wait for the script thread to finish (should be done since otherwise we would be blocked on the update() call)
    CPPUNIT_ASSERT_EQUAL(
        std::future_status::ready,
        script.wait_for(std::chrono::seconds(5))
    );

    CPPUNIT_ASSERT_MESSAGE("'text' widget was not found", text_done);
    CPPUNIT_ASSERT_MESSAGE("'path' widget was not found", path_done);
    CPPUNIT_ASSERT_MESSAGE("'file' widget was not found", file_done);
    CPPUNIT_ASSERT_MESSAGE("'checkbox' widget was not found", checkbox_done);
    CPPUNIT_ASSERT_MESSAGE("'int' widget was not found", int_done);
    CPPUNIT_ASSERT_MESSAGE("'double' widget was not found", double_done);
    CPPUNIT_ASSERT_MESSAGE("'list' widget was not found", list_done);
    CPPUNIT_ASSERT_MESSAGE("'combobox' widget was not found", combobox_done);

    // Should be true...
    CPPUNIT_ASSERT(script.get());

    // Check preferences
    {
        sight::ui::preferences preferences;
        CPPUNIT_ASSERT_EQUAL(std::string("modified"), preferences.get<std::string>("text"));
        CPPUNIT_ASSERT_EQUAL(std::string("modified"), preferences.get<std::string>("path"));
        CPPUNIT_ASSERT_EQUAL(std::string("modified"), preferences.get<std::string>("file"));
        CPPUNIT_ASSERT_EQUAL(std::string("false"), preferences.get<std::string>("checkbox"));
        CPPUNIT_ASSERT_EQUAL(std::string("42"), preferences.get<std::string>("int"));
        CPPUNIT_ASSERT_EQUAL(std::string("42.0"), preferences.get<std::string>("double"));
        CPPUNIT_ASSERT_EQUAL(std::string("3;2;1"), preferences.get<std::string>("list"));
        CPPUNIT_ASSERT_EQUAL(std::string("banana"), preferences.get<std::string>("combobox"));
    }

    // Cleanup
    CPPUNIT_ASSERT_NO_THROW(preferences_configuration->stop().get());
}

//------------------------------------------------------------------------------

void preferences_configuration::preferences_changed_after_start_test()
{
    static const std::string s_KEY = "patient_folders_root";

    // Register the service
    sight::service::base::sptr preferences_configuration(
        service::add("sight::module::ui::qt::preferences_configuration", m_child_uuid)
    );

    // Will stop the service and unregister it when destroyed
    service_cleaner cleaner(preferences_configuration);

    // Build preferences_configuration configuration
    service::config_t preference;
    preference.put("type", "path");
    preference.put("name", "Patient folders root");
    preference.put("key", s_KEY);
    preference.put("default_value", "default_value");

    service::config_t config;
    config.put_child("preference", preference);

    // Configure and start the service
    CPPUNIT_ASSERT_NO_THROW(preferences_configuration->configure(config));
    CPPUNIT_ASSERT_NO_THROW(preferences_configuration->start().get());

    // Modify the preference outside the service
    {
        sight::ui::preferences preferences;
        preferences.put(s_KEY, "/home/green");
    }

    // Start a thread to wait for the dialog to appear and click on the OK button
    auto clicker = click_on_button("OK", "preferences_configuration");

    // Update the service: will show the dialog with an the OK button
    CPPUNIT_ASSERT_EQUAL(
        std::future_status::ready,
        preferences_configuration->update().wait_for(std::chrono::seconds(10))
    );

    // Wait for the clicker thread to finish (should be done since otherwise we would be blocked on the update() call)
    CPPUNIT_ASSERT_EQUAL(
        std::future_status::ready,
        clicker.wait_for(std::chrono::seconds(5))
    );

    // Should be true and should not block because we asserted there is no timeout just before
    CPPUNIT_ASSERT_MESSAGE(
        "The button should have been clicked",
        clicker.get()
    );

    // Modify the preference outside the service
    {
        sight::ui::preferences preferences;
        preferences.put(s_KEY, "/home/house");
    }

    // Create a slot to receive the preference_changed signal
    auto slot = sight::core::com::new_slot(
        [](sight::ui::parameter_t _value, std::string _key)
        {
            if(_key == s_KEY)
            {
                CPPUNIT_ASSERT(std::holds_alternative<std::string>(_value));
                CPPUNIT_ASSERT_EQUAL(std::string("/home/house"), std::get<std::string>(_value));
            }
        });

    auto worker = core::thread::worker::make();
    slot->set_worker(worker);
    auto signal     = preferences_configuration->signal("preference_changed");
    auto connection = signal->connect(slot);

    // Request the service internal state (should execute the "slot")
    CPPUNIT_ASSERT_NO_THROW(preferences_configuration->slot("request_values")->run());

    // Start again a thread to wait for the dialog to appear and click on the OK button
    clicker = click_on_button("OK", "preferences_configuration");

    // Update the service a second time: will show the dialog with an the OK button -> and should not crash
    CPPUNIT_ASSERT_EQUAL(
        std::future_status::ready,
        preferences_configuration->update().wait_for(std::chrono::seconds(10))
    );

    /// Wait for the clicker thread to finish (should be done since otherwise we would be blocked on the update() call)
    CPPUNIT_ASSERT_EQUAL(
        std::future_status::ready,
        clicker.wait_for(std::chrono::seconds(5))
    );

    // Should be true and should not block because we asserted there is no timeout just before
    CPPUNIT_ASSERT_MESSAGE(
        "The button should have been clicked",
        clicker.get()
    );

    // Cleanup
    CPPUNIT_ASSERT_NO_THROW(preferences_configuration->stop().get());
    CPPUNIT_ASSERT_NO_THROW(worker->stop());
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
