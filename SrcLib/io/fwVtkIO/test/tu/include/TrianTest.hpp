/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWVTKIO_UT_TRIANTEST_HPP_
#define _FWVTKIO_UT_TRIANTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

namespace fwVtkIO
{
namespace ut
{

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

} // namespace ut
} // namespace fwVtkIO

#endif //_FWVTKIO_UT_TRIANTEST_HPP_
