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

#ifndef __FWDATAIO_UT_TRIANTEST_HPP__
#define __FWDATAIO_UT_TRIANTEST_HPP__

#include <boost/filesystem/operations.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

namespace fwDataIO
{
namespace ut
{

/**
 * @brief Test many methods to read/create sight trian mesh file.
 */
class TrianTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( TrianTest );
CPPUNIT_TEST( test_1 );
CPPUNIT_TEST( test_2 );
CPPUNIT_TEST( file_load_test );
CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void test_1();
    void test_2();
    void file_load_test();

protected:

    void generateTrian(::boost::filesystem::path trianFile);

    ::boost::filesystem::path m_tmpTrianPath1;
    ::boost::filesystem::path m_tmpTrianPath2;
};

} //namespace ut
} //namespace fwDataIO

#endif //__FWDATAIO_UT_TRIANTEST_HPP__
