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

#include "SFolderSelectorTest.hpp"

#include <core/com/slot.hxx>

#include <service/op.hpp>

#include <ui/__/dialog/location_dummy.hpp>
#include <ui/__/macros.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::io::ut::SFolderSelectorTest);

SIGHT_REGISTER_GUI(sight::ui::dialog::location_dummy, sight::ui::dialog::location_base::REGISTRY_KEY);

namespace sight::module::ui::io::ut
{

//------------------------------------------------------------------------------

void SFolderSelectorTest::setUp()
{
    m_folderSelector = service::add("sight::module::ui::io::SFolderSelector");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::io::SFolderSelector'", m_folderSelector);
}

//------------------------------------------------------------------------------

void SFolderSelectorTest::tearDown()
{
    m_worker->stop();
    if(!m_folderSelector->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_folderSelector->stop().get());
    }

    service::remove(m_folderSelector);
}

//------------------------------------------------------------------------------

void SFolderSelectorTest::basicTest()
{
    CPPUNIT_ASSERT_NO_THROW(m_folderSelector->configure());
    CPPUNIT_ASSERT_NO_THROW(m_folderSelector->start().get());

    std::filesystem::path path;
    auto folderSelectedSlot = core::com::new_slot(
        [&path](std::filesystem::path _path)
        {
            path = _path;
        });
    m_worker = core::thread::worker::make();
    folderSelectedSlot->set_worker(m_worker);
    m_folderSelector->signal("folderSelected")->connect(folderSelectedSlot);

    sight::ui::dialog::location_dummy::setPaths({std::filesystem::temp_directory_path()});

    CPPUNIT_ASSERT_NO_THROW(m_folderSelector->update().get());
    SIGHT_TEST_WAIT(std::filesystem::temp_directory_path() == path);
    CPPUNIT_ASSERT_EQUAL(std::filesystem::temp_directory_path(), path);

    CPPUNIT_ASSERT(sight::ui::dialog::location_dummy::clear());
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::io::ut
