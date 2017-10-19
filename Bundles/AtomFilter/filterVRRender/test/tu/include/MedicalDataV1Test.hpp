/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FILTERVRRENDER_UT_MEDICALDATAV1TEST_HPP__
#define __FILTERVRRENDER_UT_MEDICALDATAV1TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

namespace filterVRRender
{
namespace ut
{

/**
 * @brief Test ::fwAtomsFilter::IFilter::apply method.
 */
class MedicalDataV1Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MedicalDataV1Test );
CPPUNIT_TEST( applyTest );
CPPUNIT_TEST_SUITE_END();

public:

    /**
     * @name Interface
     * @{ */
    void setUp();
    void tearDown();
    /**  @} */

    void applyTest();

};

} // namespace ut
} // namespace filterVRRender

#endif //__FILTERVRRENDER_UT_MEDICALDATAV1TEST_HPP__
