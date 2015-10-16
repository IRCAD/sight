/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_MTLOCKTEST_HPP__
#define __FWDATA_UT_MTLOCKTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/String.hpp>

namespace fwData
{
namespace ut
{

class MTLockTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( MTLockTest );
    CPPUNIT_TEST( lockTest );
    CPPUNIT_TEST( multipleLockTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void lockTest();
    void multipleLockTest();

private:
    void runLock();
    void runMultipleLock1();
    void runMultipleLock2();

    ::fwData::String::sptr m_string;
};

} //namespace ut
} //namespace fwData
#endif //__FWDATA_UT_MTLOCKTEST_HPP__
