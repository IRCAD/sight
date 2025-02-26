/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "signal_shortcut_test.hpp"

#include "loader.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::signal_shortcut_test);

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

void signal_shortcut_test::setUp()
{
    std::tie(m_container, m_child_uuid) = make_container();
}

//------------------------------------------------------------------------------

void signal_shortcut_test::tearDown()
{
    // Destroy container.
    destroy_container(m_container);
}

//------------------------------------------------------------------------------

void signal_shortcut_test::basic_test()
{
    // Register the service
    sight::service::base::sptr signal_shortcut(
        service::add("sight::module::ui::qt::com::signal_shortcut", m_child_uuid)
    );

    // Build configuration
    service::config_t config;
    config.put("<xmlattr>.shortcut", "CTRL+F2");
    config.put("<xmlattr>.sid", "Test");

    config.add_child("config", config);

    // Will stop the service and unregister it when destroyed.
    service_cleaner cleaner(signal_shortcut);

    CPPUNIT_ASSERT_NO_THROW(signal_shortcut->configure(config));
    CPPUNIT_ASSERT_NO_THROW(signal_shortcut->start().get());

    // @todo: find a way to emulate the shortcut ?
    // using QTest ?

    // Cleanup
    CPPUNIT_ASSERT_NO_THROW(signal_shortcut->stop().get());
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
