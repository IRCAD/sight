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

#include "folder_selector_test.hpp"

#include <core/com/slot.hxx>

#include <service/op.hpp>

#include <ui/__/dialog/location_dummy.hpp>
#include <ui/__/macros.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::io::ut::folder_selector_test);

SIGHT_REGISTER_GUI(sight::ui::dialog::location_dummy, sight::ui::dialog::location_base::REGISTRY_KEY);

namespace sight::module::ui::io::ut
{

//------------------------------------------------------------------------------

void folder_selector_test::setUp()
{
    m_folder_selector = service::add("sight::module::ui::io::folder_selector");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::io::folder_selector'", m_folder_selector);
}

//------------------------------------------------------------------------------

void folder_selector_test::tearDown()
{
    m_worker->stop();
    if(!m_folder_selector->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_folder_selector->stop().get());
    }

    service::remove(m_folder_selector);
}

//------------------------------------------------------------------------------

void folder_selector_test::basic_test()
{
    CPPUNIT_ASSERT_NO_THROW(m_folder_selector->configure());
    CPPUNIT_ASSERT_NO_THROW(m_folder_selector->start().get());

    std::filesystem::path path;
    auto folder_selected_slot = core::com::new_slot(
        [&path](std::filesystem::path _path)
        {
            path = _path;
        });
    m_worker = core::thread::worker::make();
    folder_selected_slot->set_worker(m_worker);
    m_folder_selector->signal("folderSelected")->connect(folder_selected_slot);

    sight::ui::dialog::location_dummy::set_paths({std::filesystem::temp_directory_path()});

    CPPUNIT_ASSERT_NO_THROW(m_folder_selector->update().get());
    SIGHT_TEST_WAIT(std::filesystem::temp_directory_path() == path);
    CPPUNIT_ASSERT_EQUAL(std::filesystem::temp_directory_path(), path);

    CPPUNIT_ASSERT(sight::ui::dialog::location_dummy::clear());
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::io::ut
