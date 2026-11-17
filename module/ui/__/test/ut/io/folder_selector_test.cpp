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

#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/test/dialog/location.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <filesystem>

namespace
{

struct folder_selector_fixture
{
    folder_selector_fixture()
    {
        m_folder_selector = sight::service::add("sight::module::ui::io::folder_selector");
        REQUIRE_MESSAGE(
            m_folder_selector,
            "Failed to create service 'sight::module::ui::io::folder_selector'"
        );
    }

    ~folder_selector_fixture()
    {
        if(m_worker)
        {
            m_worker->stop();
        }

        if(!m_folder_selector->stopped())
        {
            CHECK_NOTHROW(m_folder_selector->stop().get());
        }

        sight::service::remove(m_folder_selector);
    }

    folder_selector_fixture(const folder_selector_fixture&)            = delete;
    folder_selector_fixture& operator=(const folder_selector_fixture&) = delete;
    folder_selector_fixture(folder_selector_fixture&&)                 = delete;
    folder_selector_fixture& operator=(folder_selector_fixture&&)      = delete;

    sight::service::base::sptr m_folder_selector;
    sight::core::thread::worker::sptr m_worker;
};

} // namespace

TEST_SUITE("sight::module::ui::io::folder_selector")
{
    TEST_CASE_FIXTURE(folder_selector_fixture, "basic")
    {
        CHECK_NOTHROW(m_folder_selector->configure());
        CHECK_NOTHROW(m_folder_selector->start().get());

        std::filesystem::path path;
        auto folder_selected_slot = sight::core::com::new_slot(
            [&path](std::filesystem::path _path)
        {
            path = _path;
        });

        m_worker = sight::core::thread::worker::make();
        folder_selected_slot->set_worker(m_worker);
        m_folder_selector->signal("folderSelected")->connect(folder_selected_slot);

        sight::ui::test::dialog::location::set_paths({std::filesystem::temp_directory_path()});

        CHECK_NOTHROW(m_folder_selector->update().get());
        SIGHT_TEST_WAIT(std::filesystem::temp_directory_path() == path);
        CHECK_EQ(std::filesystem::temp_directory_path(), path);

        CHECK(sight::ui::test::dialog::location::clear());
    }
}
