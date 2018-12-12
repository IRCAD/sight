/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
