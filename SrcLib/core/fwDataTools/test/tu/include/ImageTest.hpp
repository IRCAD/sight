/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_TEST_TU_IMAGETEST_HPP_
#define _FWDATATOOLS_TEST_TU_IMAGETEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Image.hpp>


namespace fwDataTools
{
namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class ImageTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageTest );
    CPPUNIT_TEST( testGetterSetter3D );
    CPPUNIT_TEST( testDeepCopy );
    CPPUNIT_TEST( stressTest );
    CPPUNIT_TEST( testGetterSetter2D );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testGetterSetter3D();
    void testDeepCopy();
    void stressTest();
    void testGetterSetter2D();

private:
    void stressTestWithType(::fwTools::Type type, int nbTest);


};

} //namespace ut
} //namespace fwDataTools

#endif //_FWDATATOOLS_TEST_TU_IMAGETEST_HPP_
