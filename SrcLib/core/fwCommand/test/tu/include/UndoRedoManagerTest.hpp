/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
