/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{
/**
 * @brief The PointListTest class
 * This class is used to test ::fwData::PointList
 */
class PointListTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( PointListTest );
    CPPUNIT_TEST( copyTest );
    CPPUNIT_TEST( getterTest );
    CPPUNIT_TEST( setterTest );
    CPPUNIT_TEST( pushTest );
    CPPUNIT_TEST( removeTest );
    CPPUNIT_TEST( clearTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void copyTest();
    void getterTest();
    void setterTest();
    void pushTest();
    void removeTest();
    void clearTest();

};

} //namespace ut
} //namespace fwData
