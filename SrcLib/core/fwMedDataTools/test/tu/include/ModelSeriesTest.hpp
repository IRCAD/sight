/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATATOOLS_UT_MODELSERIESTEST_HPP__
#define __FWMEDDATATOOLS_UT_MODELSERIESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedDataTools
{
namespace ut
{

class ModelSeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ModelSeriesTest );
CPPUNIT_TEST(addReconstruction);
CPPUNIT_TEST(addMesh);
CPPUNIT_TEST(createReconstructionFromMesh);
CPPUNIT_TEST_SUITE_END();

public:
    /**
     * @name Override
     * @{ */
    void setUp();
    void tearDown();
    /**  @} */

    void addReconstruction();
    void createReconstructionFromMesh();
    void addMesh();
};

} // namespace ut

} // namespace fwMedDataTools

#endif // __FWMEDDATATOOLS_UT_MODELSERIESTEST_HPP__
