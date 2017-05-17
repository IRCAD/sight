/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_IMAGEDIFFTEST_HPP__
#define __FWDATATOOLS_UT_IMAGEDIFFTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

/**
 * @brief Test ImageDiff storage and application.
 */
class ImageDiffTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImageDiffTest );
CPPUNIT_TEST( storeDiffsTest );
CPPUNIT_TEST( undoRedoTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    /// Test ImageDiff constructors, copy/move operators, accessors, and modifying methods.
    void storeDiffsTest();

    /// Test ImageDiff revert/apply methods.
    void undoRedoTest();
};

} //namespace ut
} //namespace fwDataTools

#endif //__FWDATATOOLS_UT_IMAGEDIFFTEST_HPP__
