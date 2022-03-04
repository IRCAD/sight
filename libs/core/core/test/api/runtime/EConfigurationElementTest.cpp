/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "EConfigurationElementTest.hpp"

#include <core/runtime/EConfigurationElement.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::runtime::ut::EConfigurationElementTest);

namespace sight::core::runtime
{

namespace ut
{

//------------------------------------------------------------------------------

void EConfigurationElementTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void EConfigurationElementTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void EConfigurationElementTest::testConfiguration()
{
    const std::string NAME1      = "config_name1";
    const std::string NAME2      = "config_name2";
    const std::string NAME3      = "config_name3";
    const std::string VALUE1     = "value1";
    const std::string VALUE2     = "value2";
    const std::string ATTRIBUTE1 = "attribute1";

    std::shared_ptr<core::runtime::EConfigurationElement> config1(new core::runtime::EConfigurationElement(NAME1));
    std::shared_ptr<core::runtime::EConfigurationElement> config2(new core::runtime::EConfigurationElement(NAME2));

    config1->addConfigurationElement(config2);
    config2->setAttributeValue(ATTRIBUTE1, VALUE1);
    config2->setValue(VALUE2);
    config2->addConfigurationElement(NAME3);

    CPPUNIT_ASSERT_EQUAL(NAME1, config1->getName());
    CPPUNIT_ASSERT_EQUAL(NAME2, config2->getName());
    CPPUNIT_ASSERT_EQUAL(VALUE2, config2->getValue());
    CPPUNIT_ASSERT(config2->hasAttribute(ATTRIBUTE1));
    CPPUNIT_ASSERT_EQUAL(VALUE1, config2->getExistingAttributeValue(ATTRIBUTE1));
    CPPUNIT_ASSERT_EQUAL(VALUE1, config2->getAttributeValue(ATTRIBUTE1));

    std::shared_ptr<core::runtime::ConfigurationElement> config3 =
        std::dynamic_pointer_cast<core::runtime::ConfigurationElement>(config2);
    CPPUNIT_ASSERT_EQUAL(config3, config1->findConfigurationElement(NAME2));
    CPPUNIT_ASSERT(config2->findConfigurationElement(NAME3) != NULL);
}

} // namespace ut

} // namespace sight::core::runtime
