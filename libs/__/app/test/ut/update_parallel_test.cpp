/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "update_parallel_test.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <ranges>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::update_parallel_test);

namespace sight::app::ut
{

/**
 * @brief Service interface for test
 */
class test_update_srv final : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_update_srv, service::base);
    ~test_update_srv() noexcept final = default;

    //------------------------------------------------------------------------------

    void configuring(const config_t& /*unused*/) final
    {
    }

    //------------------------------------------------------------------------------

    void starting() final
    {
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
        m_updated = true;
    }

    bool m_updated {false};
};

SIGHT_REGISTER_SERVICE(sight::service::base, sight::app::ut::test_update_srv);

//------------------------------------------------------------------------------

auto create_srv()
{
    auto srv = service::add<sight::app::ut::test_update_srv>("sight::app::ut::test_update_srv");
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().get());
    return srv;
}

//------------------------------------------------------------------------------

void update_parallel_test::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("app_ut");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
}

//------------------------------------------------------------------------------

void update_parallel_test::tearDown()
{
}

//------------------------------------------------------------------------------

void update_parallel_test::basic_test()
{
    auto srv0 = create_srv();
    auto srv1 = create_srv();
    auto srv2 = create_srv();
    auto srv3 = create_srv();

    std::stringstream srv_config;
    srv_config
    << "<config>"
    << "<service uid=" << std::quoted(srv0->get_id()) << "/>"
    << "<service uid=" << std::quoted(srv1->get_id()) << "/>"
    << "<service uid=" << std::quoted(srv2->get_id()) << "/>"
    << "<service uid=" << std::quoted(srv3->get_id()) << "/>"
    << "</config>";
    service::config_t config;
    boost::property_tree::read_xml(srv_config, config);

    auto update_srv = service::add("sight::app::update_parallel");
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::update_parallel"));
    CPPUNIT_ASSERT(update_srv->is_a("sight::app::updater"));
    update_srv->set_config(config);
    CPPUNIT_ASSERT_NO_THROW(update_srv->configure());
    CPPUNIT_ASSERT_NO_THROW(update_srv->start().get());

    CPPUNIT_ASSERT_NO_THROW(update_srv->update().get());
    CPPUNIT_ASSERT_EQUAL(true, srv0->m_updated);
    CPPUNIT_ASSERT_EQUAL(true, srv1->m_updated);
    CPPUNIT_ASSERT_EQUAL(true, srv2->m_updated);
    CPPUNIT_ASSERT_EQUAL(true, srv3->m_updated);

    CPPUNIT_ASSERT_NO_THROW(update_srv->stop().get());
    service::remove(update_srv);
}

//------------------------------------------------------------------------------

void update_parallel_test::parent_test()
{
    std::array<sight::app::ut::test_update_srv::sptr, 7> srv;
    for(const auto i : std::views::iota(0U, 7U))
    {
        srv[i] = create_srv();
    }

    const auto create_updater = [](std::stringstream& _config)
                                {
                                    service::config_t srv_config;
                                    boost::property_tree::read_xml(_config, srv_config);

                                    auto updater = service::add("sight::app::update_parallel");
                                    updater->set_config(srv_config);
                                    CPPUNIT_ASSERT_NO_THROW(updater->configure());
                                    CPPUNIT_ASSERT_NO_THROW(updater->start().get());
                                    return updater;
                                };

    sight::service::base::sptr main_updater;
    sight::service::base::sptr child_updater_1;
    sight::service::base::sptr child_updater_2;
    sight::service::base::sptr child_updater_1_1;

    const std::string updater_1   = "updater_1";
    const std::string updater_2   = "updater_2";
    const std::string updater_1_1 = "updater_1_1";
    {
        std::stringstream srv_config;
        srv_config
        << "<config>"
        << "<service uid=" << std::quoted(srv[0]->get_id()) << "/>"
        << "<updater uid=" << std::quoted(updater_1) << "/>"
        << "<service uid=" << std::quoted(srv[3]->get_id()) << "/>"
        << "<updater uid=" << std::quoted(updater_2) << "/>"
        << "<service uid=" << std::quoted(srv[6]->get_id()) << "/>"
        << "</config>";
        main_updater = create_updater(srv_config);
    }
    {
        std::stringstream srv_config;
        srv_config
        << "<config parent=" << std::quoted(updater_1) << ">"
        << "<updater uid=" << std::quoted(updater_1_1) << "/>"
        << "<service uid=" << std::quoted(srv[2]->get_id()) << "/>"
        << "</config>";
        child_updater_1 = create_updater(srv_config);
    }
    {
        std::stringstream srv_config;
        srv_config
        << "<config parent=" << std::quoted(updater_2) << ">"
        << "<service uid=" << std::quoted(srv[4]->get_id()) << "/>"
        << "<service uid=" << std::quoted(srv[5]->get_id()) << "/>"
        << "</config>";
        child_updater_2 = create_updater(srv_config);
    }
    {
        std::stringstream srv_config;
        srv_config
        << "<config parent=" << std::quoted(updater_1_1) << ">"
        << "<service uid=" << std::quoted(srv[1]->get_id()) << "/>"
        << "</config>";
        child_updater_1_1 = create_updater(srv_config);
    }

    main_updater->update().get();

    for(const auto i : std::views::iota(0U, 7U))
    {
        CPPUNIT_ASSERT_EQUAL(true, srv[i]->m_updated);
    }

    CPPUNIT_ASSERT_NO_THROW(main_updater->stop().get());
}

} // namespace sight::app::ut
