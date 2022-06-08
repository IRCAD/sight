/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "IActionTest.hpp"

#include <core/runtime/Convert.hpp>

#include <ui/base/IAction.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::base::ut::IActionTest);

namespace sight::ui::base
{

namespace ut
{

class TestAction : public ui::base::IAction
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

void IActionTest::setUp()
{
}

//------------------------------------------------------------------------------

void IActionTest::tearDown()
{
}

//------------------------------------------------------------------------------

void IActionTest::configuringTest()
{
    {
        auto action = std::make_shared<TestAction>();
        action->configure();

        CPPUNIT_ASSERT_EQUAL(false, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        CPPUNIT_ASSERT_EQUAL(true, action->visible());
        CPPUNIT_ASSERT_EQUAL(false, action->inverted());
    }
    {
        auto action = std::make_shared<TestAction>();
        std::stringstream xmlConfig;
        xmlConfig << ""
                     "<state visible=\"false\" checked=\"true\" enabled=\"true\" />"
                     "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xmlConfig, config);

        action->setConfiguration(config);
        action->configure();

        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        CPPUNIT_ASSERT_EQUAL(false, action->visible());
        CPPUNIT_ASSERT_EQUAL(false, action->inverted());

        CPPUNIT_ASSERT_EQUAL(true, action->confirmAction());
    }
    {
        auto action = std::make_shared<TestAction>();
        std::stringstream xmlConfig;
        xmlConfig << ""
                     "<state visible=\"false\" checked=\"true\" enabled=\"true\" />"
                     "<confirmation message=\"Are you sure?\" defaultButton=\"true\"/>"
                     "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xmlConfig, config);

        action->setConfiguration(config);
        action->configure();

        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        CPPUNIT_ASSERT_EQUAL(false, action->visible());
        CPPUNIT_ASSERT_EQUAL(false, action->inverted());

        CPPUNIT_ASSERT_EQUAL(false, action->confirmAction());
    }
    {
        // Test deprecated attributes
        auto action = std::make_shared<TestAction>();
        std::stringstream xmlConfig;
        xmlConfig << ""
                     "<state inverse=\"true\" active=\"true\" executable=\"false\" />"
                     "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xmlConfig, config);

        action->setConfiguration(config);
        action->configure();

        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        CPPUNIT_ASSERT_EQUAL(false, action->enabled());
        CPPUNIT_ASSERT_EQUAL(true, action->visible());
        CPPUNIT_ASSERT_EQUAL(true, action->inverted());
        CPPUNIT_ASSERT_EQUAL(true, action->isVisible());
        CPPUNIT_ASSERT_EQUAL(true, action->isInverted());
    }
}

//------------------------------------------------------------------------------

void IActionTest::propertiesTest()
{
    {
        auto action = std::make_shared<TestAction>();
        action->configure();

        CPPUNIT_ASSERT_EQUAL(false, action->checked());
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        CPPUNIT_ASSERT_EQUAL(true, action->isVisible());
        CPPUNIT_ASSERT_EQUAL(false, action->isInverted());

        action->start();

        using bool_slot_t = core::com::Slot<void (bool)>;

        action->slot("disable")->run();
        CPPUNIT_ASSERT_EQUAL(false, action->enabled());
        action->slot("enable")->run();
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("setEnabled"))->run(false);
        CPPUNIT_ASSERT_EQUAL(false, action->enabled());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("setEnabled"))->run(true);
        CPPUNIT_ASSERT_EQUAL(true, action->enabled());

        action->slot("check")->run();
        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        action->slot("uncheck")->run();
        CPPUNIT_ASSERT_EQUAL(false, action->checked());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("setChecked"))->run(true);
        CPPUNIT_ASSERT_EQUAL(true, action->checked());
        std::dynamic_pointer_cast<bool_slot_t>(action->slot("setChecked"))->run(false);
        CPPUNIT_ASSERT_EQUAL(false, action->checked());

        action->slot("hide")->run();
        CPPUNIT_ASSERT_EQUAL(false, action->isVisible());
        action->slot("show")->run();
        CPPUNIT_ASSERT_EQUAL(true, action->isVisible());

        {
            // Deprecated slots
            action->slot("setInexecutable")->run();
            CPPUNIT_ASSERT_EQUAL(false, action->enabled());
            CPPUNIT_ASSERT_EQUAL(false, action->getIsExecutable());
            action->slot("setExecutable")->run();
            CPPUNIT_ASSERT_EQUAL(true, action->enabled());
            CPPUNIT_ASSERT_EQUAL(true, action->getIsExecutable());
            std::dynamic_pointer_cast<bool_slot_t>(action->slot("setIsExecutable"))->run(false);
            CPPUNIT_ASSERT_EQUAL(false, action->enabled());
            CPPUNIT_ASSERT_EQUAL(false, action->getIsExecutable());
            std::dynamic_pointer_cast<bool_slot_t>(action->slot("setIsExecutable"))->run(true);
            CPPUNIT_ASSERT_EQUAL(true, action->enabled());
            CPPUNIT_ASSERT_EQUAL(true, action->getIsExecutable());

            action->slot("activate")->run();
            CPPUNIT_ASSERT_EQUAL(true, action->checked());
            CPPUNIT_ASSERT_EQUAL(true, action->getIsActive());
            action->slot("deactivate")->run();
            CPPUNIT_ASSERT_EQUAL(false, action->checked());
            CPPUNIT_ASSERT_EQUAL(false, action->getIsActive());
            std::dynamic_pointer_cast<bool_slot_t>(action->slot("setIsActive"))->run(true);
            CPPUNIT_ASSERT_EQUAL(true, action->checked());
            CPPUNIT_ASSERT_EQUAL(true, action->getIsActive());
            std::dynamic_pointer_cast<bool_slot_t>(action->slot("setIsActive"))->run(false);
            CPPUNIT_ASSERT_EQUAL(false, action->checked());
            CPPUNIT_ASSERT_EQUAL(false, action->getIsActive());
        }
        action->stop();
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::ui::base
