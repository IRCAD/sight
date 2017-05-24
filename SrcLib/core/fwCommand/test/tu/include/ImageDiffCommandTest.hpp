/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMMAND_UT_IMAGEDIFFCOMMANDTEST_HPP__
#define __FWCOMMAND_UT_IMAGEDIFFCOMMANDTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCommand
{
namespace ut
{

class ImageDiffCommandTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImageDiffCommandTest );
CPPUNIT_TEST(undoredoTest);
CPPUNIT_TEST(getSizeTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // Test
    void undoredoTest();
    void getSizeTest();
};

} //namespace ut
} //namespace fwCommand

#endif // __FWCOMMAND_UT_IMAGEDIFFCOMMANDTEST_HPP__
