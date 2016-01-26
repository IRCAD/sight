/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2004-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATATOOLS_UT_FUNCTIONSTEST_HPP__
#define __FWMEDDATATOOLS_UT_FUNCTIONSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedDataTools
{
namespace ut
{

class FunctionsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( FunctionsTest );
CPPUNIT_TEST(generatePatientIdTest);
CPPUNIT_TEST(generateStudyInstanceUidTest);
CPPUNIT_TEST_SUITE_END();

public:
    /**
     * @name Override
     * @{ */
    void setUp();
    void tearDown();
    /**  @} */

    /**
     * @name Tests UID generation methods by checking length and content.
     * @{ */
    void generatePatientIdTest();
    void generateStudyInstanceUidTest();
    /**  @} */
};

} // namespace ut

} // namespace fwMedDataTools

#endif // __FWMEDDATATOOLS_UT_FUNCTIONSTEST_HPP__

