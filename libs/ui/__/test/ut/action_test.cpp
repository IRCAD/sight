/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "action_test.hpp"

#include <ui/__/action.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::ut::action_test);

namespace sight::ui::ut
{

class test_action : public ui::action
{
public:

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

//------------------------------------------------------------------------------

void action_test::setUp()
{
}

//------------------------------------------------------------------------------

void action_test::tearDown()
{
}

//------------------------------------------------------------------------------

void action_test::configuring_test()
{
    {
        auto action = std::make_shared<test_action>();
        action->configure();

        CPPUNIT_ASSERT_EQUAL(false, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        CPPUNIT_ASSERT_EQUAL(true, action->visible());
        CPPUNIT_ASSERT_EQUAL(false, action->inverted());
    }
    {
        auto action = std::make_shared<test_action>();
        std::stringstream xml_config;
        xml_config << ""
                      "<state visible=\"false\" checked=\"true\" enabled=\"true\" />"
                      "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xml_config, config);

        action->set_config(config);
        action->configure();
        action->start();

        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        CPPUNIT_ASSERT_EQUAL(false, action->visible());
        CPPUNIT_ASSERT_EQUAL(false, action->inverted());

        CPPUNIT_ASSERT_EQUAL(true, action->confirm_action());

        action->stop();
    }
    {
        auto action = std::make_shared<test_action>();
        std::stringstream xml_config;
        xml_config << ""
                      "<state visible=\"false\" checked=\"true\" enabled=\"true\" />"
                      "<confirmation message=\"Are you sure?\" defaultButton=\"true\"/>"
                      "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xml_config, config);

        action->set_config(config);
        action->configure();
        action->start();

        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        CPPUNIT_ASSERT_EQUAL(false, action->visible());
        CPPUNIT_ASSERT_EQUAL(false, action->inverted());

        CPPUNIT_ASSERT_EQUAL(false, action->confirm_action());

        action->stop();
    }
    {
        // Test deprecated attributes
        auto action = std::make_shared<test_action>();
        std::stringstream xml_config;
        xml_config << ""
                      "<state inverse=\"true\" checked=\"true\" enabled=\"false\" />"
                      "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xml_config, config);

        action->set_config(config);
        action->configure();
        action->start();

        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        CPPUNIT_ASSERT_EQUAL(false, action->enabled());
        CPPUNIT_ASSERT_EQUAL(true, action->visible());
        CPPUNIT_ASSERT_EQUAL(true, action->inverted());

        action->stop();
    }
}

//------------------------------------------------------------------------------

void action_test::properties_test()
{
    {
        auto action = std::make_shared<test_action>();
        action->configure();

        CPPUNIT_ASSERT_EQUAL(false, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());

        action->start();

        using bool_slot_t = core::com::slot<void (bool)>;

        action->slot("disable")->run();
        CPPUNIT_ASSERT_EQUAL(false, action->enabled());
        action->slot("enable")->run();
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_enabled"))->run(false);
        CPPUNIT_ASSERT_EQUAL(false, action->enabled());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_enabled"))->run(true);
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());

        action->slot("check")->run();
        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        action->slot("uncheck")->run();
        CPPUNIT_ASSERT_EQUAL(false, action->checked());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_checked"))->run(true);
        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("set_checked"))->run(false);
        CPPUNIT_ASSERT_EQUAL(false, action->checked());

        action->slot("hide")->run();
        CPPUNIT_ASSERT_EQUAL(false, action->visible());
        action->slot("show")->run();
        CPPUNIT_ASSERT_EQUAL(true, action->visible());

        action->stop();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::ui::ut
