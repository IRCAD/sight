/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <doctest/doctest.h>

namespace sight::app::ut
{

// NOLINTNEXTLINE(misc-use-internal-linkage) Fails under windows because the id can't be parsed by boost
class test_update_srv final : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_update_srv, sight::service::base);
    ~test_update_srv() noexcept final = default;

//------------------------------------------------------------------------------

protected:

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

public:

    bool m_updated {false};
};

} // namespace sight::app::ut

SIGHT_REGISTER_SERVICE(sight::service::base, sight::app::ut::test_update_srv);

//------------------------------------------------------------------------------

static auto create_srv()
{
    auto srv = sight::service::add<sight::app::ut::test_update_srv>("sight::app::ut::test_update_srv");
    CHECK_NOTHROW(srv->configure());
    CHECK_NOTHROW(srv->start().get());
    return srv;
}

namespace
{

//------------------------------------------------------------------------------

struct fixture
{
    fixture()
    {
        // Set up context before running a test
        sight::core::runtime::init();

        std::filesystem::path location = sight::core::runtime::get_resource_file_path("app_ut");
        CHECK(std::filesystem::exists(location));

        sight::core::runtime::add_modules(location);
        sight::core::runtime::load_module("sight::module::app");
    }
};

} // namespace

TEST_SUITE("sight::app::update_parallel")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "basic")
    {
        auto srv0 = create_srv();
        auto srv1 = create_srv();
        auto srv2 = create_srv();
        auto srv3 = create_srv();

        const std::string srv_config = std::format(
            "<config>"
            "<service uid='{}'/>"
            "<service uid='{}'/>"
            "<service uid='{}'/>"
            "<service uid='{}'/>"
            "</config>",
            srv0->get_id(),
            srv1->get_id(),
            srv2->get_id(),
            srv3->get_id()
        );

        auto update_srv = sight::service::add("sight::app::update_parallel");
        CHECK(update_srv->is_a("sight::app::update_parallel"));
        CHECK(update_srv->is_a("sight::app::updater"));
        update_srv->set_config(srv_config);
        CHECK_NOTHROW(update_srv->configure());
        CHECK_NOTHROW(update_srv->start().get());

        CHECK_NOTHROW(update_srv->update().get());
        CHECK_EQ(true, srv0->m_updated);
        CHECK_EQ(true, srv1->m_updated);
        CHECK_EQ(true, srv2->m_updated);
        CHECK_EQ(true, srv3->m_updated);

        CHECK_NOTHROW(update_srv->stop().get());
        CHECK_NOTHROW(srv0->stop().get());
        CHECK_NOTHROW(srv1->stop().get());
        CHECK_NOTHROW(srv2->stop().get());
        CHECK_NOTHROW(srv3->stop().get());

        sight::service::remove(update_srv);
        sight::service::remove(srv0);
        sight::service::remove(srv1);
        sight::service::remove(srv2);
        sight::service::remove(srv3);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "parent")
    {
        std::array<sight::app::ut::test_update_srv::sptr, 7> srv;
        for(const auto i : std::views::iota(0U, 7U))
        {
            srv[i] = create_srv();
        }

        const auto create_updater = [](const std::string& _config)
                                    {
                                        auto updater = sight::service::add("sight::app::update_parallel");
                                        updater->set_config(_config);
                                        CHECK_NOTHROW(updater->configure());
                                        CHECK_NOTHROW(updater->start().get());
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
            const std::string srv_config = std::format(
                "<config>"
                "<service uid='{}'/>"
                "<updater uid='{}'/>"
                "<service uid='{}'/>"
                "<updater uid='{}'/>"
                "<service uid='{}'/>"
                "</config>",
                srv[0]->get_id(),
                updater_1,
                srv[3]->get_id(),
                updater_2,
                srv[6]->get_id()
            );
            main_updater = create_updater(srv_config);
        }
        {
            const std::string srv_config = std::format(
                "<config parent='{}'>"
                "<updater uid='{}'/>"
                "<service uid='{}'/>"
                "</config>",
                updater_1,
                updater_1_1,
                srv[2]->get_id()
            );
            child_updater_1 = create_updater(srv_config);
        }
        {
            const std::string srv_config = std::format(
                "<config parent='{}'>"
                "<service uid='{}'/>"
                "<service uid='{}'/>"
                "</config>",
                updater_2,
                srv[4]->get_id(),
                srv[5]->get_id()
            );
            child_updater_2 = create_updater(srv_config);
        }
        {
            const std::string srv_config = std::format(
                "<config parent='{}'>"
                "<service uid='{}'/>"
                "</config>",
                updater_1_1,
                srv[1]->get_id()
            );
            child_updater_1_1 = create_updater(srv_config);
        }

        main_updater->update().get();

        for(const auto i : std::views::iota(0U, 7U))
        {
            CHECK_EQ(true, srv[i]->m_updated);
        }

        CHECK_NOTHROW(main_updater->stop().get());
        CHECK_NOTHROW(child_updater_1->stop().get());
        CHECK_NOTHROW(child_updater_1_1->stop().get());
        CHECK_NOTHROW(child_updater_2->stop().get());

        sight::service::remove(main_updater);
        sight::service::remove(child_updater_1);
        sight::service::remove(child_updater_1_1);
        sight::service::remove(child_updater_2);

        for(const auto i : std::views::iota(0U, 7U))
        {
            CHECK_NOTHROW(srv[i]->stop().get());
            sight::service::remove(srv[i]);
        }
    }
} // TEST_SUITE
