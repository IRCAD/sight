/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWVTKIO_TEST_TU_TRIANTEST_HPP_
#define _FWVTKIO_TEST_TU_TRIANTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>


/**
 * @brief Test many methods to create mesh or image with VTK.
 */
class TrianTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TrianTest );
    CPPUNIT_TEST( testTrianToVtk );
    CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void testTrianToVtk();

};

#endif //_FWVTKIO_TEST_TU_TRIANTEST_HPP_
