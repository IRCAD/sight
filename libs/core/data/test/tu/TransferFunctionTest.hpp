/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include <data/TransferFunction.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data
{
namespace ut
{

class TransferFunctionTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( TransferFunctionTest );
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

    data::TransferFunction::sptr createTFColor();
    void checkTFColor( data::TransferFunction::sptr tf );
};
} //namespace ut
} //namespace sight::data
