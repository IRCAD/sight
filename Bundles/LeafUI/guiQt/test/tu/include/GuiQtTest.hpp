/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_TEST_TU_MESHGENERATORTEST_HPP_
#define _FWDATATOOLS_TEST_TU_MESHGENERATORTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Mesh.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Array.hpp>

/**
 * @brief Test many methods to create mesh.
 */
class GuiQtTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( GuiQtTest );
    CPPUNIT_TEST( testDefaultFrame );
    CPPUNIT_TEST( testTuto01 );
    CPPUNIT_TEST( testTuto02 );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testDefaultFrame();
    void testTuto01();
    void testTuto02();

};

#endif //_FWDATATOOLS_TEST_TU_MESHGENERATORTEST_HPP_
