/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwData/PointList.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

class PointListTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( PointListTest );
    CPPUNIT_TEST( computeDistance );
    CPPUNIT_TEST( transform );
    CPPUNIT_TEST( associate );
    CPPUNIT_TEST( removeClosestPointNominal );
    CPPUNIT_TEST( removeClosestPointExtreme );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void computeDistance();

    void transform();

    void associate();

    void removeClosestPointNominal();

    void removeClosestPointExtreme();
};
} //namespace ut
} //namespace fwDataTools
