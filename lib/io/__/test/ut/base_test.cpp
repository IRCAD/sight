/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <doctest/doctest.h>

#include <algorithm>

namespace
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

    sight::io::service::path_type_t m_path_type {sight::io::service::path_type_t::file};
    inline static const std::string DEFAULT_WINDOW_TITLE = "test_writer";
    std::string m_used_window_title;

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

protected:

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

protected:

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

public:

    sight::io::service::path_type_t m_path_type {sight::io::service::path_type_t::file};
    inline static const std::string DEFAULT_WINDOW_TITLE = "test_reader";
    std::string m_used_window_title;
};

} // namespace

TEST_SUITE("sight::io::base")
{
//------------------------------------------------------------------------------

    TEST_CASE("file")
    {
        // Nominal tests
        {
            const std::filesystem::path expected_file("a");
            auto file = std::make_shared<sight::data::string>(expected_file.string());

            // Reader
            {
                auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::file);
                test_reader_srv->set_input(file, sight::io::service::FILE_KEY);

                CHECK_NOTHROW(test_reader_srv->configure());
                CHECK_NOTHROW(test_reader_srv->start().get());
                CHECK_NOTHROW(test_reader_srv->update().get());
                CHECK_NOTHROW(test_reader_srv->stop().get());
                CHECK_EQ(expected_file, test_reader_srv->get_file());
                CHECK_THROWS_AS(test_reader_srv->get_files(), sight::core::exception);
                CHECK_THROWS_AS(test_reader_srv->get_folder(), sight::core::exception);
            }

            // Writer
            {
                auto test_writer_srv = std::make_shared<test_writer>(sight::io::service::file);
                test_writer_srv->set_input(file, sight::io::service::FILE_KEY);

                CHECK_NOTHROW(test_writer_srv->configure());
                CHECK_NOTHROW(test_writer_srv->start().get());
                CHECK_NOTHROW(test_writer_srv->update().get());
                CHECK_NOTHROW(test_writer_srv->stop().get());
                CHECK_EQ(expected_file, test_writer_srv->get_file());
                CHECK_THROWS_AS(test_writer_srv->get_files(), sight::core::exception);
                CHECK_THROWS_AS(test_writer_srv->get_folder(), sight::core::exception);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("files")
    {
        // Nominal tests
        {
            const sight::io::service::locations_t expected_files {"a", "b", "c"};
            auto files = std::make_shared<sight::data::string>(
                expected_files[0].string() + ";" + expected_files[1].string() + ";" + expected_files[2].string()
            );

            // Reader
            {
                auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::files);
                test_reader_srv->set_input(files, sight::io::service::FILE_KEY);

                CHECK_NOTHROW(test_reader_srv->configure());
                CHECK_NOTHROW(test_reader_srv->start().get());
                CHECK_NOTHROW(test_reader_srv->update().get());
                CHECK_NOTHROW(test_reader_srv->stop().get());

                const auto& actual_files = test_reader_srv->get_files();
                CHECK_EQ(expected_files.size(), actual_files.size());
                for(std::size_t i = 0 ; i < expected_files.size() ; ++i)
                {
                    CHECK_EQ(expected_files[i], actual_files[i]);
                }

                CHECK_THROWS_AS(test_reader_srv->get_file(), sight::core::exception);
                CHECK_THROWS_AS(test_reader_srv->get_folder(), sight::core::exception);
            }

            // Writer
            {
                auto test_writer_srv = std::make_shared<test_writer>(sight::io::service::files);
                test_writer_srv->set_input(files, sight::io::service::FILE_KEY);

                CHECK_NOTHROW(test_writer_srv->configure());
                CHECK_NOTHROW(test_writer_srv->start().get());
                CHECK_NOTHROW(test_writer_srv->update().get());
                CHECK_NOTHROW(test_writer_srv->stop().get());

                const auto& actual_files = test_writer_srv->get_files();
                CHECK_EQ(expected_files.size(), actual_files.size());
                for(std::size_t i = 0 ; i < expected_files.size() ; ++i)
                {
                    CHECK_EQ(expected_files[i], actual_files[i]);
                }

                CHECK_THROWS_AS(test_writer_srv->get_file(), sight::core::exception);
                CHECK_THROWS_AS(test_writer_srv->get_folder(), sight::core::exception);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("folder")
    {
        // Nominal tests
        {
            const std::filesystem::path expected_folder("d");
            auto folder = std::make_shared<sight::data::string>(expected_folder.string());

            // Reader
            {
                auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::folder);
                test_reader_srv->set_input(folder, sight::io::service::FOLDER_KEY);

                CHECK_NOTHROW(test_reader_srv->configure());
                CHECK_NOTHROW(test_reader_srv->start().get());
                CHECK_NOTHROW(test_reader_srv->update().get());
                CHECK_NOTHROW(test_reader_srv->stop().get());
                CHECK_EQ(expected_folder, test_reader_srv->get_folder());
                CHECK_THROWS_AS(test_reader_srv->get_file(), sight::core::exception);
                CHECK_THROWS_AS(test_reader_srv->get_files(), sight::core::exception);
            }

            // Writer
            {
                auto test_writer_srv = std::make_shared<test_writer>(sight::io::service::folder);
                test_writer_srv->set_input(folder, sight::io::service::FOLDER_KEY);

                CHECK_NOTHROW(test_writer_srv->configure());
                CHECK_NOTHROW(test_writer_srv->start().get());
                CHECK_NOTHROW(test_writer_srv->update().get());
                CHECK_NOTHROW(test_writer_srv->stop().get());
                CHECK_EQ(expected_folder, test_writer_srv->get_folder());
                CHECK_THROWS_AS(test_writer_srv->get_file(), sight::core::exception);
                CHECK_THROWS_AS(test_writer_srv->get_files(), sight::core::exception);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("resource")
    {
        // File resource
        {
            const std::string expected_resource("sight::module::ui::icons/export.svg");
            auto resource        = std::make_shared<sight::data::string>(expected_resource);
            auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::file);
            test_reader_srv->set_input(resource, sight::io::service::READER_RESOURCE_KEY);

            CHECK_NOTHROW(test_reader_srv->configure());
            CHECK_NOTHROW(test_reader_srv->start().get());
            CHECK_NOTHROW(test_reader_srv->update().get());
            CHECK_NOTHROW(test_reader_srv->stop().get());
            CHECK_EQ(
                std::filesystem::path(expected_resource).filename(),
                test_reader_srv->get_file().filename()
            );
            CHECK(std::filesystem::is_regular_file(test_reader_srv->get_file()));
            CHECK_THROWS_AS(test_reader_srv->get_files(), sight::core::exception);
            CHECK_THROWS_AS(test_reader_srv->get_folder(), sight::core::exception);

            // Reset everything
            test_reader_srv = std::make_shared<test_reader>(sight::io::service::file);

            // Test that file get precedence over resources
            const std::filesystem::path expected_file("a");
            auto file = std::make_shared<sight::data::string>(expected_file.string());
            test_reader_srv->set_input(file, sight::io::service::FILE_KEY);
            test_reader_srv->set_input(resource, sight::io::service::READER_RESOURCE_KEY);

            CHECK_NOTHROW(test_reader_srv->configure());
            CHECK_NOTHROW(test_reader_srv->start().get());
            CHECK_NOTHROW(test_reader_srv->update().get());
            CHECK_NOTHROW(test_reader_srv->stop().get());

            CHECK_EQ(expected_file, test_reader_srv->get_file());
            CHECK_THROWS_AS(test_reader_srv->get_files(), sight::core::exception);
            CHECK_THROWS_AS(test_reader_srv->get_folder(), sight::core::exception);
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

            const sight::io::service::locations_t expected_files {"a", "b", "c"};
            auto files = std::make_shared<sight::data::string>(
                expected_files[0].string() + ";" + expected_files[1].string() + ";" + expected_files[2].string()
            );

            auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::files);
            test_reader_srv->set_input(resources, sight::io::service::READER_RESOURCE_KEY);
            test_reader_srv->set_input(files, sight::io::service::FILE_KEY);

            CHECK_NOTHROW(test_reader_srv->configure());
            CHECK_NOTHROW(test_reader_srv->start().get());
            CHECK_NOTHROW(test_reader_srv->update().get());
            CHECK_NOTHROW(test_reader_srv->stop().get());

            const auto& actual_files = test_reader_srv->get_files();
            CHECK_EQ(expected_resources.size() + expected_files.size(), actual_files.size());

            // We must find all expected files
            for(const auto& expected_file : expected_files)
            {
                CHECK(std::ranges::find(actual_files, expected_file) != actual_files.cend());
            }

            // We must find all expected resources
            for(const auto& expected_resource : expected_resources)
            {
                const auto& found_file = std::ranges::find_if(
                    actual_files,
                    [&expected_resource](const std::filesystem::path& _path)
                {
                    return std::filesystem::path(expected_resource).filename() == _path.filename();
                });

                CHECK(found_file != actual_files.cend());
                CHECK(std::filesystem::is_regular_file(*found_file));
            }

            CHECK_THROWS_AS(test_reader_srv->get_file(), sight::core::exception);
            CHECK_THROWS_AS(test_reader_srv->get_folder(), sight::core::exception);
        }

        // Folder resource
        {
            const std::string expected_resource("sight::module::ui::icons/");
            auto resource        = std::make_shared<sight::data::string>(expected_resource);
            auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::folder);
            test_reader_srv->set_input(resource, sight::io::service::READER_RESOURCE_KEY);

            CHECK_NOTHROW(test_reader_srv->configure());
            CHECK_NOTHROW(test_reader_srv->start().get());
            CHECK_NOTHROW(test_reader_srv->update().get());
            CHECK_NOTHROW(test_reader_srv->stop().get());
            CHECK(test_reader_srv->get_folder().string().ends_with("icons"));
            CHECK(std::filesystem::is_directory(test_reader_srv->get_folder()));
            CHECK_THROWS_AS(test_reader_srv->get_file(), sight::core::exception);
            CHECK_THROWS_AS(test_reader_srv->get_files(), sight::core::exception);

            // Reset everything
            test_reader_srv = std::make_shared<test_reader>(sight::io::service::folder);

            // Test that folder get precedence over resources
            const std::filesystem::path expected_folder("d");
            auto folder = std::make_shared<sight::data::string>(expected_folder.string());
            test_reader_srv->set_input(folder, sight::io::service::FOLDER_KEY);
            test_reader_srv->set_input(resource, sight::io::service::READER_RESOURCE_KEY);

            CHECK_NOTHROW(test_reader_srv->configure());
            CHECK_NOTHROW(test_reader_srv->start().get());
            CHECK_NOTHROW(test_reader_srv->update().get());
            CHECK_NOTHROW(test_reader_srv->stop().get());

            CHECK_EQ(expected_folder, test_reader_srv->get_folder());
            CHECK_THROWS_AS(test_reader_srv->get_file(), sight::core::exception);
            CHECK_THROWS_AS(test_reader_srv->get_files(), sight::core::exception);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("open_location_dialog")
    {
        // Default class window title
        {
            auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::file);
            CHECK_NOTHROW(test_reader_srv->configure());
            CHECK_NOTHROW(test_reader_srv->start().get());
            CHECK_NOTHROW(test_reader_srv->open_location_dialog());
            CHECK_NOTHROW(test_reader_srv->stop().get());

            CHECK_EQ(test_reader_srv->DEFAULT_WINDOW_TITLE, test_reader_srv->m_used_window_title);

            auto test_writer_srv = std::make_shared<test_writer>(sight::io::service::file);
            CHECK_NOTHROW(test_writer_srv->configure());
            CHECK_NOTHROW(test_writer_srv->start().get());
            CHECK_NOTHROW(test_writer_srv->open_location_dialog());
            CHECK_NOTHROW(test_writer_srv->stop().get());

            CHECK_EQ(test_writer_srv->DEFAULT_WINDOW_TITLE, test_writer_srv->m_used_window_title);
        }

        // Custom window title
        {
            auto window_title = std::make_shared<sight::data::string>("Custom window title");

            auto test_reader_srv = std::make_shared<test_reader>(sight::io::service::file);
            test_reader_srv->set_input(window_title, sight::io::service::WINDOW_TITLE_KEY);
            CHECK_NOTHROW(test_reader_srv->configure());
            CHECK_NOTHROW(test_reader_srv->start().get());
            CHECK_NOTHROW(test_reader_srv->open_location_dialog());
            CHECK_NOTHROW(test_reader_srv->stop().get());

            CHECK_EQ(window_title->get_value(), test_reader_srv->m_used_window_title);

            auto test_writer_srv = std::make_shared<test_writer>(sight::io::service::file);
            test_writer_srv->set_input(window_title, sight::io::service::WINDOW_TITLE_KEY);
            CHECK_NOTHROW(test_writer_srv->configure());
            CHECK_NOTHROW(test_writer_srv->start().get());
            CHECK_NOTHROW(test_writer_srv->open_location_dialog());
            CHECK_NOTHROW(test_writer_srv->stop().get());

            CHECK_EQ(window_title->get_value(), test_writer_srv->m_used_window_title);
        }
    }
} // TEST_SUITE
