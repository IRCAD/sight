/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_UT_MESHTEST_HPP__
#define __FWRENDEROGRE_UT_MESHTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwRenderOgre
{
namespace helper
{

namespace ut
{

class MeshTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MeshTest );
CPPUNIT_TEST( copyColors );
CPPUNIT_TEST_SUITE_END();

public:
    MeshTest();
    virtual ~MeshTest();
    // interface
    void setUp();
    void tearDown();

    void copyColors();

private:
};

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_UT_MESHTEST_HPP__
