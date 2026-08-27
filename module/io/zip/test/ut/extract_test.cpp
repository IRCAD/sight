/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <core/notification/observer.hpp>
#include <core/os/temp_path.hpp>

#include <io/vtk/vti_image_reader.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/test/dialog/input.hpp>
#include <ui/test/dialog/location.hpp>
#include <ui/test/dialog/message.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::io::zip::extract")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic_archive")
    {
        const sight::core::os::temp_dir tmp_folder;

        sight::service::base::sptr extract = sight::service::add("sight::module::io::zip::extract");
        CHECK(extract);

        CHECK_NOTHROW(extract->configure());
        CHECK_NOTHROW(extract->start().get());

        // We select the archive we want to open.
        sight::ui::test::dialog::location::push_paths(
            {sight::utest_data::dir() / "sight/ui/archive_extractor/non-encrypted-archive.sample"
            });

        // We choose the output path.
        sight::ui::test::dialog::location::push_paths({tmp_folder});

        CHECK_NOTHROW(extract->update().get());

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
        CHECK_EQ(std::size_t(1), nb_vti_files);
        CHECK(!vti_path.empty());

        // Try to open the file using VTK to check if it is valid.
        auto vti_reader = std::make_shared<sight::io::vtk::vti_image_reader>();
        vti_reader->set_file(vti_path);
        auto img = std::make_shared<sight::data::image>();
        vti_reader->set_object(img);
        auto observer = std::make_shared<sight::core::notification::observer>("Reading VTI image");
        CHECK_NOTHROW(vti_reader->read(observer));

        sight::ui::test::dialog::location::push_paths(
            {sight::utest_data::dir() / "sight/ui/archive_extractor/non-encrypted-archive.sample"
            });
        sight::ui::test::dialog::location::push_paths({tmp_folder});

        // Oops, we choose the same folder again! We get a warning. Let's try again.
        sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::retry);
        sight::ui::test::dialog::location::push_paths({tmp_folder});

        // Ah, clumsy us, we chose the exact same folder! Let's try again later.
        sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::cancel);

        CHECK_NOTHROW(extract->update().get());

        sight::ui::test::dialog::location::push_paths(
            {sight::utest_data::dir() / "sight/ui/archive_extractor/non-encrypted-archive.sample"
            });
        sight::ui::test::dialog::location::push_paths({tmp_folder});

        // Well, well, the folder still isn't empty. Tough luck. Let's simply overwrite it.
        sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::yes);

        CHECK_NOTHROW(extract->update().get());

        CHECK(sight::ui::test::dialog::location::clear());
        CHECK(sight::ui::test::dialog::message::clear());

        CHECK_NOTHROW(extract->stop().get());
    }

//------------------------------------------------------------------------------

    TEST_CASE("encrypted_archive")
    {
        const sight::core::os::temp_dir tmp_folder;

        sight::service::base::sptr extract = sight::service::add("sight::module::io::zip::extract");
        CHECK(extract);

        CHECK_NOTHROW(extract->configure());
        CHECK_NOTHROW(extract->start().get());

        // We select the archive we want to open.
        sight::ui::test::dialog::location::push_paths(
            {sight::utest_data::dir() / "sight/ui/archive_extractor/encrypted-archive.sample"
            });

        // We choose the output path.
        sight::ui::test::dialog::location::push_paths({tmp_folder});

        // The archive is encrypted, let's input a password.
        sight::ui::test::dialog::input::push_input("tartare");

        // Ah, wrong one. Let's try again.
        sight::ui::test::dialog::message::push_action(sight::ui::test::dialog::message::retry);
        sight::ui::test::dialog::input::push_input("bouboule");

        CHECK_NOTHROW(extract->update().get());

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
        CHECK_EQ(std::size_t(1), nb_vti_files);
        CHECK(!vti_path.empty());

        // Try to open the file using VTK to check if it is valid.
        auto vti_reader = std::make_shared<sight::io::vtk::vti_image_reader>();
        vti_reader->set_file(vti_path);
        auto img = std::make_shared<sight::data::image>();
        vti_reader->set_object(img);
        auto observer = std::make_shared<sight::core::notification::observer>("Reading VTI image");
        CHECK_NOTHROW(vti_reader->read(observer));

        CHECK(sight::ui::test::dialog::location::clear());
        CHECK(sight::ui::test::dialog::input::clear());
        CHECK(sight::ui::test::dialog::message::clear());

        CHECK_NOTHROW(extract->stop().get());
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
