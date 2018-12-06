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

#ifndef __EIGENTOOLS_UT_EIGENTOOLSTEST_HPP__
#define __EIGENTOOLS_UT_EIGENTOOLSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <algorithm>
#include <cmath>

namespace eigenTools
{
namespace ut
{

class EigenToolsTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( EigenToolsTest );
    CPPUNIT_TEST(f4sToEigen);
    CPPUNIT_TEST(eigenToF4s);
    CPPUNIT_TEST(float16ToEigen);
    CPPUNIT_TEST(eigenMatToRvecTvec);
    CPPUNIT_TEST(f4sMatToRvecTvec);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void f4sToEigen();
    void eigenToF4s();

    void float16ToEigen();

    void eigenMatToRvecTvec();
    void f4sMatToRvecTvec();

    //------------------------------------------------------------------------------

    template<class T>
    static const T random(const T& a, const T& b)
    {
        return static_cast<T>(rand()) / static_cast<T>(RAND_MAX) * (b - a) + a;
    }
};

} //namespace ut
} //namespace eigenTools
#endif //__EIGENTOOLS_UT_EIGENTOOLSTEST_HPP__
