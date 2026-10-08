/************************************************************************
 *
 * Copyright (C) 2016-2026 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include <ui/__/action.hpp>

#include <doctest/doctest.h>

namespace sight::ui::ut
{

namespace
{

class test_action : public ui::action
{
protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
        this->initialize();
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }
};

} // namespace

TEST_SUITE("sight::ui::ut::action")
{
//------------------------------------------------------------------------------

    TEST_CASE("configuring_test")
    {
        {
            auto action = std::make_shared<test_action>();
            action->configure();

            CHECK_EQ(false, action->checked());
            CHECK_EQ(true, action->enabled());
            CHECK_EQ(true, action->visible());
            CHECK_EQ(false, action->inverted());
        }
        {
            auto action              = std::make_shared<test_action>();
            const std::string config = "<state visible='false' checked='true' enabled='true' />";

            action->set_config(config);
            action->configure();
            action->start();

            CHECK_EQ(true, action->checked());
            CHECK_EQ(true, action->enabled());
            CHECK_EQ(false, action->visible());
            CHECK_EQ(false, action->inverted());

            CHECK_EQ(true, action->confirm_action());

            action->stop();
        }
        {
            auto action              = std::make_shared<test_action>();
            const std::string config = "<state visible='false' checked='true' enabled='true' />"
                                       "<confirmation message='Are you sure?' defaultButton='true'/>";

            action->set_config(config);
            action->configure();
            action->start();

            CHECK_EQ(true, action->checked());
            CHECK_EQ(true, action->enabled());
            CHECK_EQ(false, action->visible());
            CHECK_EQ(false, action->inverted());

            CHECK_EQ(false, action->confirm_action());

            action->stop();
        }
        {
            // Test deprecated attributes
            auto action              = std::make_shared<test_action>();
            const std::string config = "<state inverse='true' checked='true' enabled='false' />";

            action->set_config(config);
            action->configure();
            action->start();

            CHECK_EQ(true, action->checked());
            CHECK_EQ(false, action->enabled());
            CHECK_EQ(true, action->visible());
            CHECK_EQ(true, action->inverted());

            action->stop();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("properties_test")
    {
        auto action = std::make_shared<test_action>();
        action->configure();

        CHECK_EQ(false, action->checked());
        CHECK_EQ(true, action->enabled());

        action->start();

        using bool_slot_t = core::com::slot<void (bool)>;

        action->slot("disable")->run();
        CHECK_EQ(false, action->enabled());
        action->slot("enable")->run();
        CHECK_EQ(true, action->enabled());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_enabled"))->run(false);
        CHECK_EQ(false, action->enabled());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_enabled"))->run(true);
        CHECK_EQ(true, action->enabled());

        action->slot("check")->run();
        CHECK_EQ(true, action->checked());
        action->slot("uncheck")->run();
        CHECK_EQ(false, action->checked());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_checked"))->run(true);
        CHECK_EQ(true, action->checked());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_checked"))->run(false);
        CHECK_EQ(false, action->checked());

        action->slot("hide")->run();
        CHECK_EQ(false, action->visible());
        action->slot("show")->run();
        CHECK_EQ(true, action->visible());

        action->stop();
    }
} // TEST_SUITE

} // namespace sight::ui::ut
