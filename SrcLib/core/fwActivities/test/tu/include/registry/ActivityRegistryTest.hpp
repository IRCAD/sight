/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_UT_REGISTRY_ACTIVITYREGISTRYTEST_HPP__
#define __FWACTIVITIES_UT_REGISTRY_ACTIVITYREGISTRYTEST_HPP__

#include <fwCore/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwActivities
{

namespace ut
{

struct ActivityRegistryTestPimpl;

class ActivityRegistryTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ActivityRegistryTest );

CPPUNIT_TEST(registryTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void registryTest();

protected:

    SPTR( ActivityRegistryTestPimpl ) m_pimpl;
};

} //namespace ut
} //namespace fwActivities

#endif //__FWACTIVITIES_UT_REGISTRY_ACTIVITYREGISTRYTEST_HPP__

