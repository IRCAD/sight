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
#include <module/io/dicom/reader.hpp>

#include <QApplication>
#include <array>

#include <core/os/temp_path.hpp>

#include <data/series_set.hpp>
#include <doctest/doctest.h>
#include <filesystem>
#include <string>

#include <io/__/service/io_types.hpp>
#include <memory>

#include <ui/test/dialog/message.hpp>

#include <utest_data/data.hpp>

//------------------------------------------------------------------------------
static void ensure_qt_application()
{
    static int argc      = 1;
    static auto app_name = std::to_array("module_io_dicom_ut");

    static std::array<char*, 2> argv = {
        app_name.data(),
        nullptr
    };
    static QApplication application(argc, argv.data());
}

//-----------------------------------------------------------------------------
static std::filesystem::path first_dicom_file(const std::filesystem::path& _folder)
{
    if(!std::filesystem::exists(_folder))
    {
        return {};
    }

    for(const auto& entry : std::filesystem::recursive_directory_iterator(_folder))
    {
        if(entry.is_regular_file())
        {
            return entry.path();
        }
    }

    return {};
}

//-----------------------------------------------------------------------------
static sight::data::series_set::sptr read_location(
    const std::filesystem::path& _location,
    const bool _is_folder,
    const std::string& _compat_filter = {})
{
    ensure_qt_application();

    auto reader = std::make_shared<sight::module::io::dicom::reader>();

    auto output = std::make_shared<sight::data::series_set>();
    reader->set_inout(output, sight::io::service::READER_DATA_KEY);

    sight::service::config_t config;
    config.put("dialog.<xmlattr>.policy", "never");

    if(!_compat_filter.empty())
    {
        config.put("dialog.<xmlattr>.filter", _compat_filter);
    }

    reader->set_config(config);

    REQUIRE_NOTHROW(reader->configure());

    if(_is_folder)
    {
        reader->set_folder(_location);
    }
    else
    {
        reader->set_files({_location});
    }

    REQUIRE_NOTHROW(reader->start().get());
    REQUIRE_NOTHROW(reader->update().get());
    REQUIRE_NOTHROW(reader->stop().get());

    return output;
}

//-----------------------------------------------------------------------------
TEST_SUITE("sight::module::io::dicom::reader")
{
    TEST_CASE("supports_files_and_folders")
    {
        const auto reader = std::make_shared<sight::module::io::dicom::reader>();

        const auto path_type = reader->get_path_type();
        CHECK((path_type& sight::io::service::files) != 0);
        CHECK((path_type& sight::io::service::folder) != 0);

        const auto extensions = reader->get_supported_extensions();
        REQUIRE(extensions.size() == 1);
        CHECK(extensions.front().first == "DICOM files");
        CHECK(extensions.front().second == "*.dcm");
    }

    //-----------------------------------------------------------------------------
    TEST_CASE("opens_a_single_dicom_file")
    {
        const auto folder = sight::utest_data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";
        const auto file   = first_dicom_file(folder);
        REQUIRE(!file.empty());

        const auto output = read_location(file, false);
        REQUIRE(output);
        REQUIRE(!output->empty());
        CHECK(output->front()->num_instances() > 0);
        CHECK(output->front()->get_file() == file);
    }

    //-----------------------------------------------------------------------------
    TEST_CASE("opens_a_dicom_folder")
    {
        const auto folder = sight::utest_data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";
        REQUIRE(std::filesystem::is_directory(folder));

        const auto output = read_location(folder, true);
        REQUIRE(output);
        CHECK(!output->empty());
    }

    //-----------------------------------------------------------------------------
    TEST_CASE("applies_the_legacy_dicom_filter")
    {
        const auto folder = sight::utest_data::dir() / "sight/Patient/Dicom/DicomDB/01-CT-DICOM_LIVER";
        REQUIRE(std::filesystem::is_directory(folder));

        sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::ok);

        // The input only contains CT images, so the legacy filter must reject it.
        const auto output = read_location(folder, true, "fiducials");
        REQUIRE(output);
        CHECK(output->empty());
        CHECK(sight::ui::test::dialog::message::clear());
    }

    //-----------------------------------------------------------------------------
    TEST_CASE("rejects_an_invalid_dicom_folder")
    {
        const sight::core::os::temp_dir folder;
        const auto missing_folder = folder / "missing";
        REQUIRE_FALSE(std::filesystem::exists(missing_folder));

        const auto output = read_location(missing_folder, true);
        REQUIRE(output);
        CHECK(output->empty());
    }
}
