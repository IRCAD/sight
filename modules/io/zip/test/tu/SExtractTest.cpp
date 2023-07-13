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

#include "SExtractTest.hpp"

#include <core/tools/System.hpp>

#include <io/vtk/VtiImageReader.hpp>

#include <service/IService.hpp>
#include <service/op/Add.hpp>

#include <ui/base/dialog/DummyInputDialog.hpp>
#include <ui/base/dialog/DummyLocationDialog.hpp>
#include <ui/base/dialog/DummyMessageDialog.hpp>
#include <ui/base/registry/macros.hpp>

#include <utestData/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::zip::ut::SExtractTest);

SIGHT_REGISTER_GUI(
    sight::ui::base::dialog::DummyLocationDialog,
    sight::ui::base::dialog::ILocationDialog::REGISTRY_KEY
);
SIGHT_REGISTER_GUI(sight::ui::base::dialog::DummyInputDialog, sight::ui::base::dialog::IInputDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::base::dialog::DummyMessageDialog, sight::ui::base::dialog::IMessageDialog::REGISTRY_KEY);

namespace sight::module::io::zip::ut
{

//------------------------------------------------------------------------------

void SExtractTest::basicArchiveTest()
{
    std::filesystem::path tmpFolder = core::tools::System::getTemporaryFolder("SExtractTest_basicArchiveTest");
    std::filesystem::remove_all(tmpFolder);
    std::filesystem::create_directories(tmpFolder);

    service::IService::sptr extract = service::add("sight::module::io::zip::SExtract");
    CPPUNIT_ASSERT(extract);

    CPPUNIT_ASSERT_NO_THROW(extract->start().get());

    // We select the archive we want to open.
    ui::base::dialog::DummyLocationDialog::pushPaths(
        {utestData::Data::dir() / "sight/ui/ArchiveExtractor/non-encrypted-archive.sample"
        });

    // We choose the output path.
    ui::base::dialog::DummyLocationDialog::pushPaths({tmpFolder});

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
    auto vtiReader = sight::io::vtk::VtiImageReader::New();
    vtiReader->setFile(vtiPath);
    auto img = data::Image::New();
    vtiReader->setObject(img);
    CPPUNIT_ASSERT_NO_THROW(vtiReader->read());

    ui::base::dialog::DummyLocationDialog::pushPaths(
        {utestData::Data::dir() / "sight/ui/ArchiveExtractor/non-encrypted-archive.sample"
        });
    ui::base::dialog::DummyLocationDialog::pushPaths({tmpFolder});

    // Oops, we choose the same folder again! We get a warning. Let's try again.
    ui::base::dialog::DummyMessageDialog::pushAction(ui::base::dialog::IMessageDialog::RETRY);
    ui::base::dialog::DummyLocationDialog::pushPaths({tmpFolder});

    // Ah, clumsy us, we chose the exact same folder! Let's try again later.
    ui::base::dialog::DummyMessageDialog::pushAction(ui::base::dialog::IMessageDialog::CANCEL);

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    ui::base::dialog::DummyLocationDialog::pushPaths(
        {utestData::Data::dir() / "sight/ui/ArchiveExtractor/non-encrypted-archive.sample"
        });
    ui::base::dialog::DummyLocationDialog::pushPaths({tmpFolder});

    // Well, well, the folder still isn't empty. Tough luck. Let's simply overwrite it.
    ui::base::dialog::DummyMessageDialog::pushAction(ui::base::dialog::IMessageDialog::YES);

    CPPUNIT_ASSERT_NO_THROW(extract->update().get());

    CPPUNIT_ASSERT(ui::base::dialog::DummyLocationDialog::clear());
    CPPUNIT_ASSERT(ui::base::dialog::DummyMessageDialog::clear());
}

//------------------------------------------------------------------------------

void SExtractTest::encryptedArchiveTest()
{
    std::filesystem::path tmpFolder = core::tools::System::getTemporaryFolder("SExtractTest_encryptedArchiveTest");
    std::filesystem::remove_all(tmpFolder);
    std::filesystem::create_directories(tmpFolder);

    service::IService::sptr extract = service::add("sight::module::io::zip::SExtract");
    CPPUNIT_ASSERT(extract);

    CPPUNIT_ASSERT_NO_THROW(extract->start().get());

    // We select the archive we want to open.
    ui::base::dialog::DummyLocationDialog::pushPaths(
        {utestData::Data::dir() / "sight/ui/ArchiveExtractor/encrypted-archive.sample"
        });

    // We choose the output path.
    ui::base::dialog::DummyLocationDialog::pushPaths({tmpFolder});

    // The archive is encrypted, let's input a password.
    ui::base::dialog::DummyInputDialog::pushInput("tartare");

    // Ah, wrong one. Let's try again.
    ui::base::dialog::DummyMessageDialog::pushAction(ui::base::dialog::IMessageDialog::RETRY);
    ui::base::dialog::DummyInputDialog::pushInput("bouboule");

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
    auto vtiReader = sight::io::vtk::VtiImageReader::New();
    vtiReader->setFile(vtiPath);
    auto img = data::Image::New();
    vtiReader->setObject(img);
    CPPUNIT_ASSERT_NO_THROW(vtiReader->read());

    CPPUNIT_ASSERT(ui::base::dialog::DummyLocationDialog::clear());
    CPPUNIT_ASSERT(ui::base::dialog::DummyInputDialog::clear());
    CPPUNIT_ASSERT(ui::base::dialog::DummyMessageDialog::clear());
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::zip::ut
