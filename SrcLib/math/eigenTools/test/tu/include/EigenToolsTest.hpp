/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
