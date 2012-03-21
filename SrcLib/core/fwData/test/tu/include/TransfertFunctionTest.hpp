/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_TRANSFERTFUNCTIONTEST_HPP__
#define __FWDATA_UT_TRANSFERTFUNCTIONTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/TransferFunction.hpp>

namespace fwData
{
namespace ut
{

class TransfertFunctionTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TransfertFunctionTest );
    CPPUNIT_TEST( constructorTest );
    CPPUNIT_TEST( defaultTfTest );
    CPPUNIT_TEST( classicGetSetTest );
    CPPUNIT_TEST( usingTfTest );
    CPPUNIT_TEST( setTFDataTest );
    CPPUNIT_TEST( shallowAndDeepCopyTest );
    CPPUNIT_TEST( linearColorTest );
    CPPUNIT_TEST( nearestColorTest);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void methode1();
    void constructorTest();
    void defaultTfTest();
    void classicGetSetTest();
    void usingTfTest();
    void setTFDataTest();
    void shallowAndDeepCopyTest();
    void linearColorTest();
    void nearestColorTest();

    ::fwData::TransferFunction::sptr createTFColor();
    void checkTFColor( ::fwData::TransferFunction::sptr tf );
};
} //namespace ut
} //namespace fwData
#endif //__FWDATA_UT_TRANSFERTFUNCTIONTEST_HPP__
