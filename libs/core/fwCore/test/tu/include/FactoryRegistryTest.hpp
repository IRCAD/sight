/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWCORE_UT_FACTORYREGISTRYTEST_HPP__
#define __FWCORE_UT_FACTORYREGISTRYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCore
{
namespace ut
{

class FactoryRegistryTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( FactoryRegistryTest );
CPPUNIT_TEST( pointerTest );
CPPUNIT_TEST( valueTest );
CPPUNIT_TEST( argTest );
CPPUNIT_TEST( threadSafetyTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void pointerTest();
    void valueTest();
    void argTest();
    void threadSafetyTest();

};

} //namespace ut
} //namespace fwCore
#endif //__FWCORE_UT_FACTORYREGISTRYTEST_HPP__
