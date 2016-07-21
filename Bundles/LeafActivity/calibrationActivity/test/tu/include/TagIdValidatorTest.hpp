/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CALIBRATIONACTIVITY_UT_TAGIDVALIDATORTEST_HPP__
#define __CALIBRATIONACTIVITY_UT_TAGIDVALIDATORTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace calibrationActivity
{
namespace ut
{

class TagIdValidatorTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( TagIdValidatorTest );
CPPUNIT_TEST( testValidatorString );
CPPUNIT_TEST( testValidatorVector );
CPPUNIT_TEST( testValidatorComposite );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testValidatorString();
    void testValidatorVector();
    void testValidatorComposite();
};

} //namespace ut
} //namespace calibrationActivity

#endif //__CALIBRATIONACTIVITY_UT_TAGIDVALIDATORTEST_HPP__
