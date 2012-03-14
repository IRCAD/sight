/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/shared_ptr.hpp>

#include "fwTools/ClassFactoryRegistry.hpp"
#include "fwTools/ClassFactory.hpp"
#include "fwTools/Object.hpp"

#include "ClassFactoryRegistryTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::ClassFactoryRegistryTest );

// Registers the fixture into the 'registry'
namespace fwTools
{
namespace ut
{

void ClassFactoryRegistryTest::setUp()
{
    // Set up context before running a test.
}

void ClassFactoryRegistryTest::tearDown()
{
    // Clean up after the test run.
}

void ClassFactoryRegistryTest::factoryRegistryTest()
{
    const std::string STR = "toto";

    ::boost::shared_ptr< ::fwTools::ClassFactory< ::fwTools::Object, ::fwTools::Object, std::string > > cf(new ::fwTools::ClassFactory< ::fwTools::Object, ::fwTools::Object, std::string >(STR));

    ::fwTools::ClassFactoryRegistry::addFactory(cf);

    std::string base = ::fwTools::getString(typeid(::fwTools::Object));
    ::fwTools::IClassFactory::sptr factory = ::fwTools::ClassFactoryRegistry::getFactory(base, STR);

    ::fwTools::Object::sptr obj = ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >(STR);

    CPPUNIT_ASSERT(factory);
    CPPUNIT_ASSERT(obj);
}

} // namespace ut
} // namespace fwTools

