/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_UT_BUFFEROBJECT_HPP__
#define __FWMEMORY_UT_BUFFEROBJECT_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMemory
{
namespace ut
{

class BufferObjectTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( BufferObjectTest );
    CPPUNIT_TEST( allocateTest );
    CPPUNIT_TEST( lockThreadedStressTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void allocateTest();
    void lockThreadedStressTest();
};

} // namespace ut
} // namespace fwMemory

#endif //__FWMEMORY_UT_BUFFEROBJECT_HPP__
