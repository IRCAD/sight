/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __FWCOMMAND_UT_UNDOREDOMANAGERTEST_HPP__
#define __FWCOMMAND_UT_UNDOREDOMANAGERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCommand
{
namespace ut
{

class UndoRedoManagerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( UndoRedoManagerTest );
CPPUNIT_TEST(managerEnqueueTest);
CPPUNIT_TEST(managerMemorySizeTest);
CPPUNIT_TEST(managerCommandCountTest);
CPPUNIT_TEST(managerClearQueueTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void managerEnqueueTest();

    void managerMemorySizeTest();

    void managerCommandCountTest();

    void managerClearQueueTest();

};

} //namespace ut
} //namespace fwCommand

#endif // __FWCOMMAND_UT_UNDOREDOMANAGERTEST_HPP__
