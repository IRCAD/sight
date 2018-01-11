/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_UT_PROCESSINGTEST_HPP__
#define __SCENE2D_UT_PROCESSINGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace scene2D
{
namespace ut
{

class ProcessingTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ProcessingTest );
CPPUNIT_TEST( histogramTest );
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void histogramTest();

};

} // namespace ut
} // namespace scene2D

#endif //__SCENE2D_UT_PROCESSINGTEST_HPP__
