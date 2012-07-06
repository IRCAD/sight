/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_UT_BUFFERMANAGER_HPP__
#define __FWMEMORY_UT_BUFFERMANAGER_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMemory
{
namespace ut
{

class BufferManagerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( BufferManagerTest );
    CPPUNIT_TEST( allocateTest );
    CPPUNIT_TEST( memoryInfoTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void allocateTest();
    void memoryInfoTest();
};

} // namespace ut
} // namespace fwMemory

#endif //__FWMEMORY_UT_BUFFERMANAGER_HPP__
