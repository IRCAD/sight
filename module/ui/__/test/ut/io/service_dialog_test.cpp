/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/location/single_folder.hpp>
#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>
#include <service/factory/new.hpp>
#include <service/registry.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/test/dialog/location.hpp>

#include <doctest/doctest.h>

#include <filesystem>

namespace sight::module::ui::io::ut
{

namespace
{

class test_reader final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(test_reader, sight::io::service::reader);

    test_reader() :
        reader("Generic reader dialog test")
    {
    }

    //------------------------------------------------------------------------------

    void set_path_type(const sight::io::service::path_type_t _path_type)
    {
        m_path_type = _path_type;
    }

    //------------------------------------------------------------------------------

    std::vector<std::pair<std::string, std::string> > get_supported_extensions() const override
    {
        return {{"Test files", "*.test"}};
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return m_path_type;
    }

protected:

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

private:

    sight::io::service::path_type_t m_path_type {sight::io::service::file};
};

class test_writer final : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(test_writer, sight::io::service::writer);

    test_writer() :
        writer("Generic writer dialog test")
    {
    }

    //------------------------------------------------------------------------------

    void set_path_type(const sight::io::service::path_type_t _path_type)
    {
        m_path_type = _path_type;
    }

    //------------------------------------------------------------------------------

    std::vector<std::pair<std::string, std::string> > get_supported_extensions() const override
    {
        return {{"Test files", "*.test"}};
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return m_path_type;
    }

    //------------------------------------------------------------------------------

    const std::string&selected_extension() const
    {
        return get_selected_extension();
    }

protected:

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

private:

    sight::io::service::path_type_t m_path_type {sight::io::service::file};
};

} // namespace

TEST_SUITE("sight::module::ui::io::service_dialog")
{
    TEST_CASE("reader_single_file_dialog")
    {
        sight::ui::test::dialog::location::clear();
        auto reader = sight::service::factory::make<test_reader>();
        sight::service::register_service(reader);
        reader->set_path_type(sight::io::service::file);
        reader->configure();
        const auto selected_path = std::filesystem::temp_directory_path() / "reader.test";
        sight::ui::test::dialog::location::set_paths({selected_path});

        reader->open_location_dialog();

        CHECK_EQ(selected_path, reader->get_file());
        CHECK_EQ(sight::ui::dialog::location::single_file, sight::ui::test::dialog::location::get_type());
        const auto options = sight::ui::test::dialog::location::get_options();
        CHECK((options& sight::ui::dialog::location::read) != 0);
        CHECK((options& sight::ui::dialog::location::file_must_exist) != 0);
        CHECK(
            std::dynamic_pointer_cast<sight::core::location::single_folder>(
                sight::ui::test::dialog::location::get_initial_location()
            )
        );
        REQUIRE_EQ(1, sight::ui::test::dialog::location::get_filters().size());
        CHECK_EQ("*.test", sight::ui::test::dialog::location::get_filters().front().second);
        CHECK(sight::ui::test::dialog::location::clear());
        sight::service::unregister_service(reader);
    }

    TEST_CASE("reader_multiple_files_dialog")
    {
        sight::ui::test::dialog::location::clear();
        auto reader = sight::service::factory::make<test_reader>();
        sight::service::register_service(reader);
        reader->set_path_type(sight::io::service::files);
        reader->configure();
        const sight::io::service::locations_t selected_paths = {
            std::filesystem::temp_directory_path() / "first.test",
            std::filesystem::temp_directory_path() / "second.test"
        };
        sight::ui::test::dialog::location::set_paths(selected_paths);

        reader->open_location_dialog();

        CHECK_EQ(selected_paths, reader->get_files());
        CHECK_EQ(sight::ui::dialog::location::multi_files, sight::ui::test::dialog::location::get_type());
        CHECK((sight::ui::test::dialog::location::get_options() & sight::ui::dialog::location::read) != 0);
        CHECK(sight::ui::test::dialog::location::clear());
        sight::service::unregister_service(reader);
    }

    TEST_CASE("reader_folder_dialog")
    {
        sight::ui::test::dialog::location::clear();
        auto reader = sight::service::factory::make<test_reader>();
        sight::service::register_service(reader);
        reader->set_path_type(sight::io::service::folder);
        reader->configure();
        const auto selected_path = std::filesystem::temp_directory_path() / "reader-input";
        sight::ui::test::dialog::location::set_paths({selected_path});

        reader->open_location_dialog();

        CHECK_EQ(selected_path, reader->get_folder());
        CHECK_EQ(sight::ui::dialog::location::folder, sight::ui::test::dialog::location::get_type());
        const auto options = sight::ui::test::dialog::location::get_options();
        CHECK((options& sight::ui::dialog::location::read) != 0);
        CHECK((options& sight::ui::dialog::location::file_must_exist) != 0);
        CHECK(sight::ui::test::dialog::location::get_filters().empty());
        CHECK(sight::ui::test::dialog::location::clear());
        sight::service::unregister_service(reader);
    }

    TEST_CASE("writer_single_file_dialog")
    {
        sight::ui::test::dialog::location::clear();
        auto writer = sight::service::factory::make<test_writer>();
        sight::service::register_service(writer);
        writer->set_path_type(sight::io::service::file);
        writer->configure();
        const auto selected_path = std::filesystem::temp_directory_path() / "writer.test";
        sight::ui::test::dialog::location::set_paths({selected_path});

        writer->open_location_dialog();

        CHECK_EQ(selected_path, writer->get_file());
        CHECK_EQ(".test", writer->selected_extension());
        CHECK_EQ(sight::ui::dialog::location::single_file, sight::ui::test::dialog::location::get_type());
        CHECK((sight::ui::test::dialog::location::get_options() & sight::ui::dialog::location::write) != 0);
        CHECK((sight::ui::test::dialog::location::get_options() & sight::ui::dialog::location::file_must_exist) == 0);
        CHECK(sight::ui::test::dialog::location::clear());
        sight::service::unregister_service(writer);
    }

    TEST_CASE("writer_folder_dialog")
    {
        sight::ui::test::dialog::location::clear();
        auto writer = sight::service::factory::make<test_writer>();
        sight::service::register_service(writer);
        writer->set_path_type(sight::io::service::folder);
        writer->configure();
        const auto selected_path = std::filesystem::temp_directory_path() / "writer-output";
        sight::ui::test::dialog::location::set_paths({selected_path});

        writer->open_location_dialog();

        CHECK_EQ(selected_path, writer->get_folder());
        CHECK_EQ(sight::ui::dialog::location::folder, sight::ui::test::dialog::location::get_type());
        CHECK((sight::ui::test::dialog::location::get_options() & sight::ui::dialog::location::write) != 0);
        CHECK(sight::ui::test::dialog::location::get_filters().empty());
        CHECK(sight::ui::test::dialog::location::clear());
        sight::service::unregister_service(writer);
    }

    TEST_CASE("writer_cancel_clears_previous_location")
    {
        sight::ui::test::dialog::location::clear();
        auto writer = sight::service::factory::make<test_writer>();
        sight::service::register_service(writer);
        writer->set_path_type(sight::io::service::file);
        writer->configure();
        writer->set_file(std::filesystem::temp_directory_path() / "previous.test");
        sight::ui::test::dialog::location::set_paths({});

        writer->open_location_dialog();

        CHECK_FALSE(writer->has_location_defined());
        CHECK(sight::ui::test::dialog::location::clear());
        sight::service::unregister_service(writer);
    }
} // TEST_SUITE

} // namespace sight::module::ui::io::ut
