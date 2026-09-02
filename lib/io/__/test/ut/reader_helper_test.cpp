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

#include <core/os/temp_path.hpp>

#include <data/image_series.hpp>
#include <data/string.hpp>

#include <io/__/reader/reader_helper.hpp>
#include <io/__/service/reader.hpp>

#include <service/macros.hpp>

#include <doctest/doctest.h>

#include <fstream>
#include <string_view>
#include <vector>

namespace sight::io::reader::ut
{

namespace
{

class test_reader final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(test_reader, sight::io::service::reader);

    test_reader() noexcept :
        reader("Reader helper test")
    {
    }

    //------------------------------------------------------------------------------

    static void reset()
    {
        s_read_paths.clear();
    }

    inline static std::vector<std::filesystem::path> s_read_paths;

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
    }

    //------------------------------------------------------------------------------

    std::vector<std::pair<std::string, std::string> > get_supported_extensions() override
    {
        return {{"Test", ".test"}};
    }

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return sight::io::service::file;
    }

protected:

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        s_read_paths.push_back(this->get_file());
        m_read_failed = false;
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }
};

SIGHT_REGISTER_SERVICE(
    sight::io::service::reader,
    sight::io::reader::ut::test_reader,
    sight::data::string
)

struct fixture
{
    //------------------------------------------------------------------------------

    std::filesystem::path make_file(std::string_view _filename)
    {
        const auto path = m_directory / _filename;
        std::ofstream file(path);
        CHECK(file.is_open());
        return path;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]]  std::filesystem::path missing_file(std::string_view _filename) const
    {
        return m_directory / _filename;
    }

    sight::core::os::temp_dir m_directory;
};

} // namespace

} // namespace sight::io::reader::ut

TEST_SUITE("sight::io::reader::reader_helper")
{
    TEST_CASE_FIXTURE(sight::io::reader::ut::fixture, "read_supported_file")
    {
        using test_reader = sight::io::reader::ut::test_reader;

        test_reader::reset();
        const auto path = make_file("input.test");
        const auto data = std::make_shared<sight::data::string>();

        CHECK(sight::io::reader::read_paths({path}, data, nullptr));
        CHECK_EQ(test_reader::s_read_paths.size(), std::size_t(1));
        CHECK_EQ(test_reader::s_read_paths.front(), path);
    }

    TEST_CASE_FIXTURE(sight::io::reader::ut::fixture, "read_multiple_supported_files")
    {
        using test_reader = sight::io::reader::ut::test_reader;

        test_reader::reset();
        const std::vector paths {make_file("first.test"), make_file("second.test")};
        const auto data = std::make_shared<sight::data::string>();

        CHECK(sight::io::reader::read_paths(paths, data, nullptr));
        CHECK_EQ(test_reader::s_read_paths, paths);
    }

    TEST_CASE_FIXTURE(sight::io::reader::ut::fixture, "reject_unsupported_file")
    {
        using test_reader = sight::io::reader::ut::test_reader;

        test_reader::reset();
        const auto path = make_file("input.unsupported");
        const auto data = std::make_shared<sight::data::string>();

        CHECK_FALSE(sight::io::reader::read_paths({path}, data, nullptr));
        CHECK(test_reader::s_read_paths.empty());
    }

    TEST_CASE_FIXTURE(sight::io::reader::ut::fixture, "reject_missing_file")
    {
        using test_reader = sight::io::reader::ut::test_reader;

        test_reader::reset();
        const auto path = missing_file("missing.test");
        const auto data = std::make_shared<sight::data::string>();

        CHECK_FALSE(sight::io::reader::read_paths({path}, data, nullptr));
        CHECK(test_reader::s_read_paths.empty());
    }

    TEST_CASE_FIXTURE(sight::io::reader::ut::fixture, "append_unique_series")
    {
        const auto first_path  = make_file("first.vtk");
        const auto second_path = make_file("second.vtk");

        auto destination = std::make_shared<sight::data::series_set>();
        auto first       = std::make_shared<sight::data::image_series>();
        first->set_file(first_path);
        destination->push_back(first);

        auto source    = std::make_shared<sight::data::series_set>();
        auto duplicate = std::make_shared<sight::data::image_series>();
        duplicate->set_file(first_path);
        source->push_back(duplicate);

        auto second = std::make_shared<sight::data::image_series>();
        second->set_file(second_path);
        source->push_back(second);

        CHECK_EQ(sight::io::reader::append_unique(*destination, *source), std::size_t(1));

        CHECK_EQ(destination->size(), std::size_t(2));
        CHECK_EQ(destination->front()->get_file(), first_path);
        CHECK_EQ(destination->back()->get_file(), second_path);
    }
}
