/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "base.hpp"

#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::base_test);

namespace sight::io::ut
{

class test_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(test_writer, sight::io::service::writer);

    explicit  test_writer(sight::io::service::path_type_t _pt) :
        writer(DEFAULT_WINDOW_TITLE),
        m_path_type(_pt)
    {
    }

    io::service::path_type_t m_path_type {io::service::path_type_t::file};

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return m_path_type;
    }

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
        m_used_window_title = *m_window_title;
    }

    inline static const std::string DEFAULT_WINDOW_TITLE = "test_writer";
    std::string m_used_window_title;
};

class test_reader : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(test_reader, sight::io::service::reader);

    explicit  test_reader(sight::io::service::path_type_t _pt) :
        reader(DEFAULT_WINDOW_TITLE),
        m_path_type(_pt)
    {
    }

    io::service::path_type_t m_path_type {io::service::path_type_t::file};

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return m_path_type;
    }

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
        m_used_window_title = *m_window_title;
    }

    inline static const std::string DEFAULT_WINDOW_TITLE = "test_reader";
    std::string m_used_window_title;
};

//------------------------------------------------------------------------------

void base_test::setUp()
{
}

//------------------------------------------------------------------------------

void base_test::tearDown()
{
}

//------------------------------------------------------------------------------

void base_test::file_test()
{
    // Nominal tests
    {
        const std::filesystem::path expected_file("a");
        auto file = std::make_shared<sight::data::string>(expected_file.string());

        // Reader
        {
            auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::file);
            test_reader->set_inout(file, sight::io::service::FILES_KEY);

            CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
            CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
            CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
            CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());
            CPPUNIT_ASSERT_EQUAL(expected_file, test_reader->get_file());
            CPPUNIT_ASSERT_THROW(test_reader->get_files(), core::exception);
            CPPUNIT_ASSERT_THROW(test_reader->get_folder(), core::exception);
        }

        // Writer
        {
            auto test_writer = std::make_shared<sight::io::ut::test_writer>(sight::io::service::file);
            test_writer->set_inout(file, sight::io::service::FILES_KEY);

            CPPUNIT_ASSERT_NO_THROW(test_writer->configure());
            CPPUNIT_ASSERT_NO_THROW(test_writer->start().wait());
            CPPUNIT_ASSERT_NO_THROW(test_writer->update().wait());
            CPPUNIT_ASSERT_NO_THROW(test_writer->stop().wait());
            CPPUNIT_ASSERT_EQUAL(expected_file, test_writer->get_file());
            CPPUNIT_ASSERT_THROW(test_writer->get_files(), core::exception);
            CPPUNIT_ASSERT_THROW(test_writer->get_folder(), core::exception);
        }
    }
}

//------------------------------------------------------------------------------

void base_test::files_test()
{
    // Nominal tests
    {
        const io::service::locations_t expected_files {"a", "b", "c"};
        auto files = std::make_shared<sight::data::string>(
            expected_files[0].string() + ";" + expected_files[1].string() + ";" + expected_files[2].string()
        );

        // Reader
        {
            auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::files);
            test_reader->set_inout(files, sight::io::service::FILES_KEY);

            CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
            CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
            CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
            CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());

            const auto& actual_files = test_reader->get_files();
            CPPUNIT_ASSERT_EQUAL(expected_files.size(), actual_files.size());
            for(std::size_t i = 0 ; i < expected_files.size() ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(expected_files[i], actual_files[i]);
            }

            CPPUNIT_ASSERT_THROW(test_reader->get_file(), core::exception);
            CPPUNIT_ASSERT_THROW(test_reader->get_folder(), core::exception);
        }

        // Writer
        {
            auto test_writer = std::make_shared<sight::io::ut::test_writer>(sight::io::service::files);
            test_writer->set_inout(files, sight::io::service::FILES_KEY);

            CPPUNIT_ASSERT_NO_THROW(test_writer->configure());
            CPPUNIT_ASSERT_NO_THROW(test_writer->start().wait());
            CPPUNIT_ASSERT_NO_THROW(test_writer->update().wait());
            CPPUNIT_ASSERT_NO_THROW(test_writer->stop().wait());

            const auto& actual_files = test_writer->get_files();
            CPPUNIT_ASSERT_EQUAL(expected_files.size(), actual_files.size());
            for(std::size_t i = 0 ; i < expected_files.size() ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(expected_files[i], actual_files[i]);
            }

            CPPUNIT_ASSERT_THROW(test_writer->get_file(), core::exception);
            CPPUNIT_ASSERT_THROW(test_writer->get_folder(), core::exception);
        }
    }
}

//------------------------------------------------------------------------------

void base_test::folder_test()
{
    // Nominal tests
    {
        const std::filesystem::path expected_folder("d");
        auto folder = std::make_shared<sight::data::string>(expected_folder.string());

        // Reader
        {
            auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::folder);
            test_reader->set_inout(folder, sight::io::service::FOLDER_KEY);

            CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
            CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
            CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
            CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());
            CPPUNIT_ASSERT_EQUAL(expected_folder, test_reader->get_folder());
            CPPUNIT_ASSERT_THROW(test_reader->get_file(), core::exception);
            CPPUNIT_ASSERT_THROW(test_reader->get_files(), core::exception);
        }

        // Writer
        {
            auto test_writer = std::make_shared<sight::io::ut::test_writer>(sight::io::service::folder);
            test_writer->set_inout(folder, sight::io::service::FOLDER_KEY);

            CPPUNIT_ASSERT_NO_THROW(test_writer->configure());
            CPPUNIT_ASSERT_NO_THROW(test_writer->start().wait());
            CPPUNIT_ASSERT_NO_THROW(test_writer->update().wait());
            CPPUNIT_ASSERT_NO_THROW(test_writer->stop().wait());
            CPPUNIT_ASSERT_EQUAL(expected_folder, test_writer->get_folder());
            CPPUNIT_ASSERT_THROW(test_writer->get_file(), core::exception);
            CPPUNIT_ASSERT_THROW(test_writer->get_files(), core::exception);
        }
    }
}

//------------------------------------------------------------------------------

void base_test::resource_test()
{
    // File resource
    {
        const std::string expected_resource("sight::module::ui::icons/export.svg");
        auto resource    = std::make_shared<sight::data::string>(expected_resource);
        auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::file);
        test_reader->set_inout(resource, sight::io::service::RESOURCES_KEY);

        CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
        CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());
        CPPUNIT_ASSERT_EQUAL(
            std::filesystem::path(expected_resource).filename(),
            test_reader->get_file().filename()
        );
        CPPUNIT_ASSERT(std::filesystem::is_regular_file(test_reader->get_file()));
        CPPUNIT_ASSERT_THROW(test_reader->get_files(), core::exception);
        CPPUNIT_ASSERT_THROW(test_reader->get_folder(), core::exception);

        // Reset everything
        test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::file);

        // Test that file get precedence over resources
        const std::filesystem::path expected_file("a");
        auto file = std::make_shared<sight::data::string>(expected_file.string());
        test_reader->set_inout(file, sight::io::service::FILES_KEY);
        test_reader->set_inout(resource, sight::io::service::RESOURCES_KEY);

        CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
        CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());

        CPPUNIT_ASSERT_EQUAL(expected_file, test_reader->get_file());
        CPPUNIT_ASSERT_THROW(test_reader->get_files(), core::exception);
        CPPUNIT_ASSERT_THROW(test_reader->get_folder(), core::exception);
    }

    // Files resource
    {
        const std::vector<std::string> expected_resources {
            "sight::module::ui::icons/export.svg",
            "sight::module::ui::icons/pull.svg",
            "sight::module::ui::icons/push.svg",
        };

        auto resources = std::make_shared<sight::data::string>(
            expected_resources[0] + ";" + expected_resources[1] + ";" + expected_resources[2]
        );

        const io::service::locations_t expected_files {"a", "b", "c"};
        auto files = std::make_shared<sight::data::string>(
            expected_files[0].string() + ";" + expected_files[1].string() + ";" + expected_files[2].string()
        );

        auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::files);
        test_reader->set_inout(resources, sight::io::service::RESOURCES_KEY);
        test_reader->set_inout(files, sight::io::service::FILES_KEY);

        CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
        CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());

        const auto& actual_files = test_reader->get_files();
        CPPUNIT_ASSERT_EQUAL(expected_resources.size() + expected_files.size(), actual_files.size());

        // We must find all expected files
        for(const auto& expected_file : expected_files)
        {
            CPPUNIT_ASSERT(std::find(actual_files.cbegin(), actual_files.cend(), expected_file) != actual_files.cend());
        }

        // We must find all expected resources
        for(const auto& expected_resource : expected_resources)
        {
            const auto& found_file = std::find_if(
                actual_files.cbegin(),
                actual_files.cend(),
                [&expected_resource](const std::filesystem::path& _path)
                {
                    return std::filesystem::path(expected_resource).filename() == _path.filename();
                });

            CPPUNIT_ASSERT(found_file != actual_files.cend());
            CPPUNIT_ASSERT(std::filesystem::is_regular_file(*found_file));
        }

        CPPUNIT_ASSERT_THROW(test_reader->get_file(), core::exception);
        CPPUNIT_ASSERT_THROW(test_reader->get_folder(), core::exception);
    }

    // Folder resource
    {
        const std::string expected_resource("sight::module::ui::icons/");
        auto resource    = std::make_shared<sight::data::string>(expected_resource);
        auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::folder);
        test_reader->set_inout(resource, sight::io::service::RESOURCES_KEY);

        CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
        CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());
        CPPUNIT_ASSERT(test_reader->get_folder().string().ends_with("icons"));
        CPPUNIT_ASSERT(std::filesystem::is_directory(test_reader->get_folder()));
        CPPUNIT_ASSERT_THROW(test_reader->get_file(), core::exception);
        CPPUNIT_ASSERT_THROW(test_reader->get_files(), core::exception);

        // Reset everything
        test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::folder);

        // Test that folder get precedence over resources
        const std::filesystem::path expected_folder("d");
        auto folder = std::make_shared<sight::data::string>(expected_folder.string());
        test_reader->set_inout(folder, sight::io::service::FOLDER_KEY);
        test_reader->set_inout(resource, sight::io::service::RESOURCES_KEY);

        CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
        CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->update().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());

        CPPUNIT_ASSERT_EQUAL(expected_folder, test_reader->get_folder());
        CPPUNIT_ASSERT_THROW(test_reader->get_file(), core::exception);
        CPPUNIT_ASSERT_THROW(test_reader->get_files(), core::exception);
    }
}

//------------------------------------------------------------------------------

void base_test::open_location_dialog_test()
{
    // Default class window title
    {
        auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::file);
        CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
        CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->open_location_dialog());
        CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());

        CPPUNIT_ASSERT_EQUAL(test_reader->DEFAULT_WINDOW_TITLE, test_reader->m_used_window_title);

        auto test_writer = std::make_shared<sight::io::ut::test_writer>(sight::io::service::file);
        CPPUNIT_ASSERT_NO_THROW(test_writer->configure());
        CPPUNIT_ASSERT_NO_THROW(test_writer->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_writer->open_location_dialog());
        CPPUNIT_ASSERT_NO_THROW(test_writer->stop().wait());

        CPPUNIT_ASSERT_EQUAL(test_writer->DEFAULT_WINDOW_TITLE, test_writer->m_used_window_title);
    }

    // Custom window title
    {
        auto window_title = std::make_shared<sight::data::string>("Custom window title");

        auto test_reader = std::make_shared<sight::io::ut::test_reader>(sight::io::service::file);
        test_reader->set_inout(window_title, sight::io::service::WINDOW_TITLE_KEY);
        CPPUNIT_ASSERT_NO_THROW(test_reader->configure());
        CPPUNIT_ASSERT_NO_THROW(test_reader->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_reader->open_location_dialog());
        CPPUNIT_ASSERT_NO_THROW(test_reader->stop().wait());

        CPPUNIT_ASSERT_EQUAL(window_title->get_value(), test_reader->m_used_window_title);

        auto test_writer = std::make_shared<sight::io::ut::test_writer>(sight::io::service::file);
        test_writer->set_inout(window_title, sight::io::service::WINDOW_TITLE_KEY);
        CPPUNIT_ASSERT_NO_THROW(test_writer->configure());
        CPPUNIT_ASSERT_NO_THROW(test_writer->start().wait());
        CPPUNIT_ASSERT_NO_THROW(test_writer->open_location_dialog());
        CPPUNIT_ASSERT_NO_THROW(test_writer->stop().wait());

        CPPUNIT_ASSERT_EQUAL(window_title->get_value(), test_writer->m_used_window_title);
    }
}

} // namespace sight::io::ut
