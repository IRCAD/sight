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

// cspell:ignore bouboule

#include "extract_test.hpp"

#include <core/tools/system.hpp>

#include <io/vtk/VtiImageReader.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/__/dialog/input_dummy.hpp>
#include <ui/__/dialog/location_dummy.hpp>
#include <ui/__/dialog/message_dummy.hpp>
#include <ui/__/macros.hpp>

#include <utest_data/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::zip::ut::extract_test);

SIGHT_REGISTER_GUI(sight::ui::dialog::location_dummy, sight::ui::dialog::location_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::dialog::input_dummy, sight::ui::dialog::input_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::dialog::message_dummy, sight::ui::dialog::message_base::REGISTRY_KEY);

namespace sight::module::io::zip::ut
{

//------------------------------------------------------------------------------

void extract_test::basicArchiveTest()
{
    std::filesystem::path tmpFolder = core::tools::system::get_temporary_folder("extract_test_basicArchiveTest");
    std::filesystem::remove_all(tmpFolder);
    std::filesystem::create_directories(tmpFolder);

    service::base::sptr extract = service::add("sight::module::io::zip::extract");
    CPPUNIT_ASSERT(extract);

    CPPUNIT_ASSERT_NO_THROW(extract->start().get());

    // We select the archive we want to open.
    ui::dialog::location_dummy::pushPaths(
        {utest_data::Data::dir() / "sight/ui/ArchiveExtractor/non-encrypted-archive.sample"
        });

    // We choose the output path.
    ui::dialog::location_dummy::pushPaths({tmpFolder});

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    // There must be precisely one VTI file inside.
    std::filesystem::path vtiPath;
    std::size_t nbVtiFiles = 0;
    std::ranges::for_each(
        std::filesystem::recursive_directory_iterator {tmpFolder},
        [&vtiPath, &nbVtiFiles](const std::filesystem::directory_entry& entry)
        {
            if(entry.path().extension() == ".vti")
            {
                nbVtiFiles++;
                vtiPath = entry;
            }
        });
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), nbVtiFiles);
    CPPUNIT_ASSERT(!vtiPath.empty());

    // Try to open the file using VTK to check if it is valid.
    auto vtiReader = std::make_shared<sight::io::vtk::VtiImageReader>();
    vtiReader->set_file(vtiPath);
    auto img = std::make_shared<data::image>();
    vtiReader->set_object(img);
    CPPUNIT_ASSERT_NO_THROW(vtiReader->read());

    ui::dialog::location_dummy::pushPaths(
        {utest_data::Data::dir() / "sight/ui/ArchiveExtractor/non-encrypted-archive.sample"
        });
    ui::dialog::location_dummy::pushPaths({tmpFolder});

    // Oops, we choose the same folder again! We get a warning. Let's try again.
    ui::dialog::message_dummy::pushAction(ui::dialog::message_dummy::RETRY);
    ui::dialog::location_dummy::pushPaths({tmpFolder});

    // Ah, clumsy us, we chose the exact same folder! Let's try again later.
    ui::dialog::message_dummy::pushAction(ui::dialog::message_dummy::CANCEL);

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    ui::dialog::location_dummy::pushPaths(
        {utest_data::Data::dir() / "sight/ui/ArchiveExtractor/non-encrypted-archive.sample"
        });
    ui::dialog::location_dummy::pushPaths({tmpFolder});

    // Well, well, the folder still isn't empty. Tough luck. Let's simply overwrite it.
    ui::dialog::message_dummy::pushAction(ui::dialog::message_dummy::YES);

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    CPPUNIT_ASSERT(ui::dialog::location_dummy::clear());
    CPPUNIT_ASSERT(ui::dialog::message_dummy::clear());
}

//------------------------------------------------------------------------------

void extract_test::encryptedArchiveTest()
{
    std::filesystem::path tmpFolder = core::tools::system::get_temporary_folder("extract_test_encryptedArchiveTest");
    std::filesystem::remove_all(tmpFolder);
    std::filesystem::create_directories(tmpFolder);

    service::base::sptr extract = service::add("sight::module::io::zip::extract");
    CPPUNIT_ASSERT(extract);

    CPPUNIT_ASSERT_NO_THROW(extract->start().get());

    // We select the archive we want to open.
    ui::dialog::location_dummy::pushPaths(
        {utest_data::Data::dir() / "sight/ui/ArchiveExtractor/encrypted-archive.sample"
        });

    // We choose the output path.
    ui::dialog::location_dummy::pushPaths({tmpFolder});

    // The archive is encrypted, let's input a password.
    ui::dialog::input_dummy::pushInput("tartare");

    // Ah, wrong one. Let's try again.
    ui::dialog::message_dummy::pushAction(ui::dialog::message_dummy::RETRY);
    ui::dialog::input_dummy::pushInput("bouboule");

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    // There must be precisely one VTI file inside.
    std::filesystem::path vtiPath;
    std::size_t nbVtiFiles = 0;
    std::ranges::for_each(
        std::filesystem::recursive_directory_iterator {tmpFolder},
        [&vtiPath, &nbVtiFiles](const std::filesystem::directory_entry& entry)
        {
            if(entry.path().extension() == ".vti")
            {
                nbVtiFiles++;
                vtiPath = entry;
            }
        });
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), nbVtiFiles);
    CPPUNIT_ASSERT(!vtiPath.empty());

    // Try to open the file using VTK to check if it is valid.
    auto vtiReader = std::make_shared<sight::io::vtk::VtiImageReader>();
    vtiReader->set_file(vtiPath);
    auto img = std::make_shared<data::image>();
    vtiReader->set_object(img);
    CPPUNIT_ASSERT_NO_THROW(vtiReader->read());

    CPPUNIT_ASSERT(ui::dialog::location_dummy::clear());
    CPPUNIT_ASSERT(ui::dialog::input_dummy::clear());
    CPPUNIT_ASSERT(ui::dialog::message_dummy::clear());
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::zip::ut
