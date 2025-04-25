/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

// cspell:ignore bouboule

#include "extract_test.hpp"

#include <core/os/temp_path.hpp>

#include <io/vtk/vti_image_reader.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/test/dialog/input.hpp>
#include <ui/test/dialog/location.hpp>
#include <ui/test/dialog/message.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::zip::ut::extract_test);

namespace sight::module::io::zip::ut
{

//------------------------------------------------------------------------------

void extract_test::basic_archive_test()
{
    const core::os::temp_dir tmp_folder;

    service::base::sptr extract = service::add("sight::module::io::zip::extract");
    CPPUNIT_ASSERT(extract);

    CPPUNIT_ASSERT_NO_THROW(extract->configure());
    CPPUNIT_ASSERT_NO_THROW(extract->start().get());

    // We select the archive we want to open.
    ui::test::dialog::location::push_paths(
        {utest_data::dir() / "sight/ui/archive_extractor/non-encrypted-archive.sample"
        });

    // We choose the output path.
    ui::test::dialog::location::push_paths({tmp_folder});

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    // There must be precisely one VTI file inside.
    std::filesystem::path vti_path;
    std::size_t nb_vti_files = 0;
    std::ranges::for_each(
        std::filesystem::recursive_directory_iterator {tmp_folder},
        [&vti_path, &nb_vti_files](const std::filesystem::directory_entry& _entry)
        {
            if(_entry.path().extension() == ".vti")
            {
                nb_vti_files++;
                vti_path = _entry;
            }
        });
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), nb_vti_files);
    CPPUNIT_ASSERT(!vti_path.empty());

    // Try to open the file using VTK to check if it is valid.
    auto vti_reader = std::make_shared<sight::io::vtk::vti_image_reader>();
    vti_reader->set_file(vti_path);
    auto img = std::make_shared<data::image>();
    vti_reader->set_object(img);
    CPPUNIT_ASSERT_NO_THROW(vti_reader->read());

    ui::test::dialog::location::push_paths(
        {utest_data::dir() / "sight/ui/archive_extractor/non-encrypted-archive.sample"
        });
    ui::test::dialog::location::push_paths({tmp_folder});

    // Oops, we choose the same folder again! We get a warning. Let's try again.
    ui::test::dialog::message::push_action(ui::test::dialog::message::retry);
    ui::test::dialog::location::push_paths({tmp_folder});

    // Ah, clumsy us, we chose the exact same folder! Let's try again later.
    ui::test::dialog::message::push_action(ui::test::dialog::message::cancel);

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    ui::test::dialog::location::push_paths(
        {utest_data::dir() / "sight/ui/archive_extractor/non-encrypted-archive.sample"
        });
    ui::test::dialog::location::push_paths({tmp_folder});

    // Well, well, the folder still isn't empty. Tough luck. Let's simply overwrite it.
    ui::test::dialog::message::push_action(ui::test::dialog::message::yes);

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    CPPUNIT_ASSERT(ui::test::dialog::location::clear());
    CPPUNIT_ASSERT(ui::test::dialog::message::clear());
}

//------------------------------------------------------------------------------

void extract_test::encrypted_archive_test()
{
    const core::os::temp_dir tmp_folder;

    service::base::sptr extract = service::add("sight::module::io::zip::extract");
    CPPUNIT_ASSERT(extract);

    CPPUNIT_ASSERT_NO_THROW(extract->configure());
    CPPUNIT_ASSERT_NO_THROW(extract->start().get());

    // We select the archive we want to open.
    ui::test::dialog::location::push_paths(
        {utest_data::dir() / "sight/ui/archive_extractor/encrypted-archive.sample"
        });

    // We choose the output path.
    ui::test::dialog::location::push_paths({tmp_folder});

    // The archive is encrypted, let's input a password.
    ui::test::dialog::input::push_input("tartare");

    // Ah, wrong one. Let's try again.
    ui::test::dialog::message::push_action(ui::test::dialog::message::retry);
    ui::test::dialog::input::push_input("bouboule");

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    // There must be precisely one VTI file inside.
    std::filesystem::path vti_path;
    std::size_t nb_vti_files = 0;
    std::ranges::for_each(
        std::filesystem::recursive_directory_iterator {tmp_folder},
        [&vti_path, &nb_vti_files](const std::filesystem::directory_entry& _entry)
        {
            if(_entry.path().extension() == ".vti")
            {
                nb_vti_files++;
                vti_path = _entry;
            }
        });
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), nb_vti_files);
    CPPUNIT_ASSERT(!vti_path.empty());

    // Try to open the file using VTK to check if it is valid.
    auto vti_reader = std::make_shared<sight::io::vtk::vti_image_reader>();
    vti_reader->set_file(vti_path);
    auto img = std::make_shared<data::image>();
    vti_reader->set_object(img);
    CPPUNIT_ASSERT_NO_THROW(vti_reader->read());

    CPPUNIT_ASSERT(ui::test::dialog::location::clear());
    CPPUNIT_ASSERT(ui::test::dialog::input::clear());
    CPPUNIT_ASSERT(ui::test::dialog::message::clear());
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::zip::ut
