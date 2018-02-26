/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwTools/Object.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwTools
{
namespace ut
{

class FwIDTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( FwIDTest );
CPPUNIT_TEST( objectFwIDTest );
CPPUNIT_TEST( concurrentAccessOnFwIDMapTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void objectFwIDTest();

    void concurrentAccessOnFwIDMapTest();

private:
    void runFwIDCreation();
};

} // namespace ut
} // namespace fwTools
