/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_MEDICALIMAGEHELPERSTEST_HPP__
#define __FWDATATOOLS_UT_MEDICALIMAGEHELPERSTEST_HPP__

#include <fwCore/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

class MedicalImageHelpersTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MedicalImageHelpersTest );
CPPUNIT_TEST( getMinMaxTest );
CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void getMinMaxTest();
};

} // namespace ut
} // namespace fwDataTools

#endif // __FWDATATOOLS_UT_MEDICALIMAGEHELPERSTEST_HPP__
