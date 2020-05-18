/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __ITKREGISTRATIONOP_UT_RESAMPLERTEST_HPP__
#define __ITKREGISTRATIONOP_UT_RESAMPLERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace itkRegistrationOp
{
namespace ut
{

class ResamplerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ResamplerTest );
CPPUNIT_TEST( identityTest );
CPPUNIT_TEST( translateTest );
//CPPUNIT_TEST( rotateTest );//fail
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void identityTest();
    void translateTest();
    void rotateTest();

};

} //namespace ut
} //namespace itkRegistrationOp

#endif // __ITKREGISTRATIONOP_UT_RESAMPLERTEST_HPP__
