/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_TEST_TU_TRIANTEST_HPP_
#define _FWDATAIO_TEST_TU_TRIANTEST_HPP_

#include <boost/filesystem/operations.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>


/**
 * @brief Test many methods to read/create fw4spl trian mesh file.
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

#endif //_FWDATAIO_TEST_TU_TRIANTEST_HPP_
