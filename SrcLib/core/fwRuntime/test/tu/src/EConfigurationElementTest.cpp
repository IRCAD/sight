/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/EConfigurationElement.hpp>

#include "EConfigurationElementTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( EConfigurationElementTest );

void EConfigurationElementTest::setUp()
{
	// Set up context before running a test.
}

void EConfigurationElementTest::tearDown()
{
	// Clean up after the test run.
}

void EConfigurationElementTest::testConfiguration()
{
	const std::string NAME1 = "config_name1";
	const std::string NAME2 = "config_name2";
	const std::string NAME3 = "config_name3";
	const std::string VALUE1 = "value1";
	const std::string VALUE2 = "value2";
	const std::string ATTRIBUTE1 = "attibute1";

	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > config1( new ::fwRuntime::EConfigurationElement(NAME1));
	::boost::shared_ptr< ::fwRuntime::EConfigurationElement > config2( new ::fwRuntime::EConfigurationElement(NAME2));

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

	::boost::shared_ptr< ::fwRuntime::ConfigurationElement > config3 = ::boost::dynamic_pointer_cast< ::fwRuntime::ConfigurationElement >(config2);
	CPPUNIT_ASSERT_EQUAL(config3, config1->findConfigurationElement(NAME2));
	CPPUNIT_ASSERT(config2->findConfigurationElement(NAME3) != NULL);
}
